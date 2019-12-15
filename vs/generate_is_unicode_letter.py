#!/usr/bin/env python3

import sys
import re
import os
import os.path
import math
import requests
import traceback



def get_script_folder():
    return os.path.dirname(os.path.realpath(sys.argv[0]))



def make_literal(codepoint):
	if (codepoint > 0xFFFF):
		return "U'\\U{:08X}'".format(codepoint)
	else:
		return "U'\\u{:04X}'".format(codepoint)



def calculate_subdivisions(span_size):

	# if it's a relatively small span, divide it such the effective size of each subchunk
	# would be less than or equal to 64 (so we'll generate bitmask ops)
	if (64 < span_size <= 4096):
		subdiv_count = int(math.ceil(span_size / 64))
		return subdiv_count

	if (4096 < span_size <= 262144):
		subdiv_count = int(math.ceil(span_size / 4096))
		return subdiv_count

	# try to find a divisor that will yield a power-of-2 size
	subdiv_count = 2
	subdiv_size = int(math.ceil(span_size / float(subdiv_count)))
	while (subdiv_count <= 64):
		if (subdiv_size > 0 and subdiv_size < span_size and (subdiv_size & (subdiv_size-1) == 0)):
			return subdiv_count
		subdiv_count += 1
		subdiv_size = int(math.ceil(span_size / float(subdiv_count)))

	# couldn't find divisor that would yield a power-of-2 size
	subdiv_count = 64
	subdiv_size = int(math.ceil(span_size / float(subdiv_count)))
	while (subdiv_count > 1):
		if (subdiv_size > 0 and subdiv_size < span_size):
			return subdiv_count
		subdiv_count -= 2
		subdiv_size = int(math.ceil(span_size / float(subdiv_count)))

	return subdiv_count



class Chunk:

	def __init__(self, first, last):
		self.first = int(first)
		self.last = int(last)
		self.span_size = (self.last - self.first) + 1
		self.count = 0
		self.ranges = []
		self.subchunks = None
		self.subchunk_count = 0
		self.subchunk_size = 0
		self.low_range_mask = 0 # first 64 only

	def add(self, first, last = None):
		f = int(first)
		num_added = 0
		if (last is None or first == last):
			self.ranges.append(f)
			num_added = 1
		else:
			l = int(last)
			self.ranges.append((f, l))
			num_added = (l - f) + 1
		self.count += num_added
		first_offset = f - self.first
		while (0 <= first_offset <= 63 and num_added > 0):
			self.low_range_mask |= (1 << first_offset)
			first_offset += 1
			num_added -= 1

	def subdivide(self):
		if (self.subchunks is not None
			or self.count == self.span_size
			or self.count == 0
			or len(self.ranges) <= 3
			):
			return
		subchunk_count = calculate_subdivisions(self.span_size)
		if (subchunk_count <= 1):
			return
		subchunk_size = int(math.ceil(self.span_size / float(subchunk_count)))
		if (subchunk_size <= 4):
			return

		self.subchunks = []
		self.subchunk_count = subchunk_count
		self.subchunk_size = subchunk_size
		for subchunk in range(subchunk_count):
			self.subchunks.append((
				subchunk,
				Chunk(
					self.first + (subchunk * self.subchunk_size),
					min(self.first + (((subchunk + 1) * self.subchunk_size) - 1), self.last)
				)
			))
		for r in self.ranges:
			if (isinstance(r, int)):
				subchunk = int((r - self.first) / self.subchunk_size)
				self.subchunks[subchunk][1].add(r)
			else:
				start_chunk = int((r[0] - self.first) / self.subchunk_size)
				end_chunk = int((r[1] - self.first) / self.subchunk_size)
				for subchunk in range(start_chunk, end_chunk+1):
					self.subchunks[subchunk][1].add(
						max(r[0], self.subchunks[subchunk][1].first),
						min(r[1], self.subchunks[subchunk][1].last),
					)
		#self.ranges = None
		for subchunk in self.subchunks:
			subchunk[1].subdivide()

	def print_subchunk(self, subchunk, subchunk_index, output_file, level, indent):
		print("{}\tcase {}u: ".format(indent, subchunk_index), end='', file=output_file)
		if (subchunk.count == subchunk.span_size):
			subchunk.print(output_file, level + 1)
		else:
			if (subchunk.subchunks is not None and subchunk.span_size > 64):
				print("\n{}\t{{".format(indent), file=output_file)
			subchunk.print(output_file, level + 1)
			if (subchunk.subchunks is not None and subchunk.span_size > 64):
				print("{}\t}}".format(indent), file=output_file)

	def print(self, output_file, level = 0):
		indent = '\t\t' + ('\t' * (2 * level))

		# return true; (completely full range)
		if (self.count == self.span_size):
			print("return true;", file=output_file)

		# return false; (completely empty range)
		elif (self.count == 0):
			print("return false;", file=output_file)
			
		# return cp == A
		elif (self.count == 1):
			print('return codepoint == {};'.format(make_literal(self.ranges[0])), file=output_file)

		# return cp & 0b000000000 (32-bit)
		elif (self.span_size <= 32):
			print('return (1u << (static_cast<uint32_t>(codepoint) - 0x{:X}u)) & 0b{:032b}u;'.format(self.first, self.low_range_mask), file=output_file)

		# return cp & 0b000000000 (64-bit)
		elif (self.span_size <= 64):
			print('return (1ull << (static_cast<uint64_t>(codepoint) - 0x{:X}ull)) & 0b{:064b}ull;'.format(self.first, self.low_range_mask), file=output_file)

		# switch (cp)
		elif (self.subchunks is not None):
			print("{}TOML_ASSUME(codepoint >= {});".format(indent, make_literal(self.first)), file=output_file)
			print("{}TOML_ASSUME(codepoint <= {});".format(indent, make_literal(self.last)), file=output_file)
			print("{}switch ((static_cast<uint32_t>(codepoint) - 0x{:X}u) / {}u)\n{}{{".format(
					indent,
					self.first,
					self.subchunk_size,
					indent
				),
				file=output_file
			)
			for subchunk_index, subchunk in self.subchunks:
				self.print_subchunk(subchunk, subchunk_index, output_file, level, indent)
			print("{}\tTOML_NO_DEFAULT_CASE;".format(indent), file=output_file)
			print("{}}}".format(indent), file=output_file)
			#print("{}return false;".format(indent), file=output_file)
			print("{}TOML_UNREACHABLE;".format(indent), file=output_file)
			print("{}// total codepoints in chunk: {}".format(indent, self.count), file=output_file)

		# return cp == A || cp == B
		else:
			print("return", end='', file=output_file)
			line_weight = 0
			first_line = True
			for range_idx in range(0, len(self.ranges)):
				r = self.ranges[range_idx]
				range_weight = (1 if (
					isinstance(r, int)
					or (range_idx == 0 and r[0] == self.first)
					or (range_idx == (len(self.ranges)-1) and r[1] == self.last))
					else 2
				)
				needs_space = True
				if ((line_weight + range_weight) > (5 - (1 if first_line else 0))):
					print("\n\t{}".format(indent), end='', file=output_file)
					line_weight = range_weight
					needs_space = False
					first_line = False
				else:
					line_weight += range_weight
				if (needs_space):
					print(" ", end='', file=output_file)
				if (range_idx > 0):
					print("|| ", end='', file=output_file)
				if (isinstance(r, int)):
					print("codepoint == {}".format(make_literal(r)), end='', file=output_file)
				elif (range_idx == 0 and r[0] == self.first):
					print("codepoint <= {}".format(make_literal(r[1])), end='', file=output_file)
				elif (range_idx == (len(self.ranges)-1) and r[1] == self.last):
					print("codepoint >= {}".format(make_literal(r[0])), end='', file=output_file)
				else:
					print("{}codepoint >= {} && codepoint <= {}{}".format(
							'(' if len(self.ranges) > 1 else '',
							make_literal(r[0]),
							make_literal(r[1]),
							')' if len(self.ranges) > 1 else ''
						),
						end='',
						file=output_file
					)
			print(";", file=output_file)



def main():

	# get unicode character database
	codepoint_list = ''
	codepoint_file_path = os.path.join(get_script_folder(), 'UnicodeData.txt')
	if (not os.path.exists(codepoint_file_path)):
		print("Couldn't find UnicodeData.txt, will download")
		response = requests.get(
			'https://www.unicode.org/Public/UCD/latest/ucd/UnicodeData.txt',
			timeout=1
		)
		codepoint_list = response.text
		codepoint_file = open(codepoint_file_path,'w') 
		print(codepoint_list, end='', file=codepoint_file)
		codepoint_file.close()
	else:
		print("Reading UnicodeData.txt into memory")
		codepoint_file = open(codepoint_file_path,'r')
		codepoint_list = codepoint_file.read()
		codepoint_file.close()

	# parse the database file into codepoints
	re_codepoint = re.compile(r'^([0-9a-fA-F]+);(.+?);([a-zA-Z]+);')
	categories = (
		'Ll', # lowercase
		'Lm', # modifiers
		'Lo', # other
		'Lt', # title case
		'Lu', # uppercase
	)
	current_range_start = -1
	codepoints = []
	for codepoint_entry in codepoint_list.split('\n'):
		match = re_codepoint.search(codepoint_entry)
		if (match is None):
			if (current_range_start > -1):
				raise Exception('Previous codepoint indicated the start of a range but the next one was null')
			continue
		codepoint = int('0x{}'.format(match.group(1)), 16)
		if (codepoint <= 128): # ASCII range is handled separately
			continue
		if (current_range_start > -1):
			for cp in range(current_range_start, codepoint):
				codepoints.append(cp)
			current_range_start = -1
		elif (match.group(3) in categories):
			if (match.group(2).endswith(', First>')):
				current_range_start = codepoint
			else:
				codepoints.append(codepoint)
	if (len(codepoints) == 0):
		print("No codepoints to add.")
		return
	print("Added {} codepoints from {} categories.".format(len(codepoints), len(categories)))
	codepoints.sort()

	# divide the codepoints up into chunks of ranges
	root_chunk = Chunk(codepoints[0], codepoints[-1])
	first_codepoint = 0
	last_codepoint = 0
	for codepoint in codepoints:
		if (first_codepoint == 0):
			first_codepoint = codepoint
			last_codepoint = codepoint
		elif (last_codepoint == codepoint-1):
			last_codepoint = codepoint
		else:
			root_chunk.add(first_codepoint, last_codepoint)
			first_codepoint = codepoint
			last_codepoint = codepoint
	if (first_codepoint != 0):
		root_chunk.add(first_codepoint, last_codepoint)
	root_chunk.subdivide()
	
	# write the output file
	output_file_path = os.path.join(get_script_folder(), '../include/toml++/toml_utf8_is_unicode_letter.h')
	print("Writing to {}".format(output_file_path))
	output_file = open(output_file_path,'w') 
	print('''// this file was generated by generate_is_unicode_letter.py
#pragma once
#include "toml_common.h"

namespace TOML_NAMESPACE::impl
{{
	[[nodiscard]]
	constexpr bool is_unicode_letter(char32_t codepoint) noexcept
	{{
		if (codepoint < {} || codepoint > {})
			return false;

'''.format(make_literal(codepoints[0]), make_literal(codepoints[-1])),
		file=output_file,
		end=''
	)
	root_chunk.print(output_file)
	print('''
	}
}''',
		file=output_file
	)
	output_file.close()



if __name__ == '__main__':
	try:
		main()
	except Exception as err:
		print(
			'Fatal error: [{}] {}'.format(
				type(err).__name__,
				str(err)
			),
			file=sys.stderr
		)
		traceback.print_exception(type(err), err, None, file=sys.stderr)
		sys.exit(1)
	sys.exit()
