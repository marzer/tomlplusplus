#!/usr/bin/env python3

import sys
import re
import os
import os.path
import math
import requests
import traceback


#### SETTINGS / MISC #########################################


class Settings:
	binary_bitmasks = False
	switch_case_limit = 64



def make_literal(codepoint):
	if (codepoint > 0xFFFF):
		return "U'\\U{:08X}'".format(codepoint)
	else:
		return "U'\\u{:04X}'".format(codepoint)



def make_bitmask(codepoint, size=64):
	if (Settings.binary_bitmasks):
		if (size==64):
			return "0b{:064b}ull".format(codepoint)
		else:
			return "0b{:032b}u".format(codepoint)
	else:
		if (size==64):
			return "0x{:X}ull".format(codepoint)
		else:
			return "0x{:X}u".format(codepoint)


def make_mask_from_indices(indices):
	mask = 0
	for i in indices:
		mask = mask | (1 << i)
	return mask;



def range_first(r):
	if isinstance(r, int):
		return r
	else:
		return r[0]



def range_last(r):
	if isinstance(r, int):
		return r
	else:
		return r[1]



def calculate_subdivisions(span_size):

	# if it's a relatively small span, divide it such the effective size of each subchunk
	# would be less than or equal to 64 so we'll generate bitmask ops
	if (64 < span_size <= 4096):
		subdiv_count = int(math.ceil(span_size / 64))
		return subdiv_count

	# try to find a divisor that will yield a power-of-2 size
	subdiv_count = 2
	subdiv_size = int(math.ceil(span_size / float(subdiv_count)))
	while (subdiv_count <= Settings.switch_case_limit):
		if (subdiv_size > 0 and subdiv_size < span_size and (subdiv_size & (subdiv_size-1) == 0)):
			return subdiv_count
		subdiv_count += 1
		subdiv_size = int(math.ceil(span_size / float(subdiv_count)))

	# couldn't find divisor that would yield a power-of-2 size
	subdiv_count = Settings.switch_case_limit
	subdiv_size = int(math.ceil(span_size / float(subdiv_count)))
	while (subdiv_count > 1):
		if (subdiv_size > 0 and subdiv_size < span_size):
			return subdiv_count
		subdiv_count -= 1
		subdiv_size = int(math.ceil(span_size / float(subdiv_count)))

	return subdiv_count



#### CHUNK ###################################################


class Chunk:

	def __init__(self, first, last):
		self.first = int(first)
		self.last = int(last)
		self.span_size = (self.last - self.first) + 1
		self.count = 0
		self.ranges = []
		self.subchunks = None
		self.subchunk_size = 0
		self.first_set = self.last + 1
		self.last_set = -1
		self.first_unset = self.first


	def low_range_mask(self):
		if self.count == 0:
			return 0;
		mask = 0
		bits = 0
		prev_last_unset = -1
		for r in self.ranges:
			first = range_first(r)
			last = range_last(r)
			count = (last - first) + 1
			while (prev_last_unset >= 0 and prev_last_unset < first and bits < 64):
				prev_last_unset += 1
				bits += 1
			if (bits >= 64):
				break;
			while (count > 0 and bits < 64):
				mask |= (1 << bits)
				bits += 1
				count -= 1
			if (bits >= 64):
				break;

			prev_last_unset = last + 1
		return mask


	def add(self, first, last = None):
		f = int(first)
		num_added = 0
		if (last is None or first == last):
			self.ranges.append(f)
			self.count += 1
			self.last_set = max(self.last_set, f)
			if (self.first_unset == f):
				self.first_unset = f + 1
		else:
			l = int(last)
			self.ranges.append((f, l))
			self.count += (l - f) + 1
			self.last_set = max(self.last_set, l)
			if (self.first_unset == f):
				self.first_unset = l + 1
		self.first_set = min(self.first_set, f)


	def trim(self):
		if (self.subchunks is not None
			or self.count == 0
			or (self.first_set == self.first and self.last_set == self.last)):
			return

		self.first = self.first_set
		self.last = self.last_set
		self.span_size = (self.last - self.first) + 1


	def subdivide(self):
		if (self.subchunks is not None
			or self.count >= self.span_size - 1
			or self.count <= 1
			or (self.last_set - self.first_set) + 1 <= 64
			or self.count == (self.last - self.first_set) + 1
			or self.count == (self.first_unset - self.first)
			or self.count == (self.last_set - self.first_set) + 1
			or (len(self.ranges) == 2 and range_first(self.ranges[0]) == self.first and range_last(self.ranges[1]) == self.last)
			or len(self.ranges) <= 4
			):
			return
		subchunk_count = calculate_subdivisions(self.span_size)
		if (subchunk_count <= 1):
			return
		subchunk_size = int(math.ceil(self.span_size / float(subchunk_count)))
		if (subchunk_size <= 4):
			return

		self.subchunks = []
		self.subchunk_size = subchunk_size
		for subchunk in range(subchunk_count):
			self.subchunks.append(
				Chunk(
					self.first + (subchunk * self.subchunk_size),
					min(self.first + (((subchunk + 1) * self.subchunk_size) - 1), self.last)
				)
			)
		for r in self.ranges:
			if (isinstance(r, int)):
				subchunk = int((r - self.first) / self.subchunk_size)
				self.subchunks[subchunk].add(r)
			else:
				start_chunk = int((r[0] - self.first) / self.subchunk_size)
				end_chunk = int((r[1] - self.first) / self.subchunk_size)
				for subchunk in range(start_chunk, end_chunk+1):
					self.subchunks[subchunk].add(
						max(r[0], self.subchunks[subchunk].first),
						min(r[1], self.subchunks[subchunk].last),
					)
		#self.ranges = None
		for subchunk in self.subchunks:
			subchunk.trim()
			subchunk.subdivide()


	def always_returns_true(self):
		return self.count == self.span_size;


	def always_returns_false(self):
		return self.count == 0;


	def print_subchunk(self, subchunk_index, output_file, level, indent):
		print("{}\tcase {}: ".format(indent, subchunk_index), end='', file=output_file)
		if (self.subchunks[subchunk_index].count == self.subchunks[subchunk_index].span_size):
			self.subchunks[subchunk_index].print(output_file, level + 1, (self.first, self.last))
		else:
			if (self.subchunks[subchunk_index].subchunks is not None and self.subchunks[subchunk_index].span_size > 64):
				print("\n{}\t{{".format(indent), file=output_file)
			self.subchunks[subchunk_index].print(output_file, level + 1, (self.first, self.last))
			if (self.subchunks[subchunk_index].subchunks is not None and self.subchunks[subchunk_index].span_size > 64):
				print("{}\t}}".format(indent), file=output_file)


	def print(self, output_file, level = 0, parent_range = None):
		indent = '\t\t' + ('\t' * (2 * level))
		if (parent_range is None):
			parent_range = (0, 0x7FFFFFFF)

		# return true; (completely full range)
		if (self.always_returns_true()):
			print("return true;", file=output_file)

		# return false; (completely empty range)
		elif (self.always_returns_false()):
			print("return false;", file=output_file)

		# return cp == A
		elif (self.count == 1):
			print('return codepoint == {};'.format(make_literal(self.ranges[0])), file=output_file)

		# return cp != A
		elif (self.count == self.span_size - 1):
			print('return codepoint != {};'.format(make_literal(self.first_unset)), file=output_file)

		# return cp < A
		elif (self.count == (self.first_unset - self.first)):
			print('return codepoint < {};'.format(make_literal(self.first_unset)), file=output_file)

		# return cp >= A
		elif (self.count == (self.last - self.first_set) + 1):
			print('return codepoint >= {};'.format(make_literal(self.first_set)), file=output_file)

		# return cp >= A && cp <= B
		elif (self.count == (self.last_set - self.first_set) + 1):
			print('return codepoint >= {} && codepoint <= {};'.format(make_literal(self.first_set), make_literal(self.last_set)), file=output_file)

		# return cp <= A || cp >= B
		elif (len(self.ranges) == 2 and range_first(self.ranges[0]) == self.first and range_last(self.ranges[1]) == self.last):
			print('return codepoint <= {} || codepoint >= {};'.format(make_literal(range_last(self.ranges[0])), make_literal(range_first(self.ranges[1]))), file=output_file)

		# return cp & A (32-bit)
		elif ((self.last_set - self.first_set) + 1 <= 32):
			if (self.first_set == self.first):
				print('return (1u << (static_cast<uint_least32_t>(codepoint) - 0x{:X}u)) & {};'.format(self.first_set, make_bitmask(self.low_range_mask(), 32)), file=output_file)
			else:
				print('return codepoint >= {} && ((1u << (static_cast<uint_least32_t>(codepoint) - 0x{:X}u)) & {});'
					.format(make_literal(self.first_set), self.first_set, make_bitmask(self.low_range_mask(), 32)), file=output_file)

		# return cp & A (64-bit)
		elif ((self.last_set - self.first_set) + 1 <= 64):
			if (self.first_set == self.first):
				print('return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x{:X}ull)) & {};'.format(self.first_set, make_bitmask(self.low_range_mask())), file=output_file)
			else:
				print('return codepoint >= {} && ((1ull << (static_cast<uint_least64_t>(codepoint) - 0x{:X}ull)) & {});'
					.format(make_literal(self.first_set), self.first_set, make_bitmask(self.low_range_mask())), file=output_file)

		# switch (cp)
		elif (self.subchunks is not None):
			if (self.first > parent_range[0] and self.last < parent_range[1]):
				print("{}if (codepoint < {} || codepoint > {})\n{}\treturn false;\n".format(indent, make_literal(self.first), make_literal(self.last), indent), file=output_file)
			elif (self.first > parent_range[0]):
				print("{}if (codepoint < {})\n{}\treturn false;\n".format(indent, make_literal(self.first), indent), file=output_file)
			elif (self.last < parent_range[1]):
				print("{}if (codepoint > {})\n{}\treturn false;\n".format(indent, make_literal(self.last), indent), file=output_file)

			# see if we can avoid emitting a switch altogether
			always_true = []
			always_false = []
			not_always_true_or_false = []
			all_true_even = True
			all_true_odd = True
			all_false_even = True
			all_false_odd = True
			for subchunk_index in range(len(self.subchunks)):
				even = (subchunk_index % 2) == 0
				if self.subchunks[subchunk_index].always_returns_true():
					always_true.append(subchunk_index)
					all_true_even = all_true_even and even
					all_true_odd = all_true_odd and not even
				elif self.subchunks[subchunk_index].always_returns_false():
					always_false.append(subchunk_index)
					all_false_even = all_false_even and even
					all_false_odd = all_false_odd and not even
				else:
					not_always_true_or_false.append(subchunk_index)

			selector = '(static_cast<uint_least32_t>(codepoint) - 0x{:X}u) / {}u'.format(self.first, self.subchunk_size)

			# return selector & mask
			if (len(always_true) + len(always_false) == len(self.subchunks) and len(self.subchunks) <= 64):
				print("{}return ({}) & {};".format(indent, selector, make_bitmask(make_mask_from_indices(always_true))), file=output_file)

			# return selector == A ? true : selector & mask
			#elif (len(not_always_true_or_false) == 1
			#	and (len(always_true) + len(always_false)) == len(self.subchunks)-1
			#	and len(self.subchunks) <= 64):
			#	print('{}const auto selector = {}; //kek'.format(indent, selector), file=output_file)
			#	print('{}return selector == {}u ? true : selector & {};'.format(
			#			indent,
			#			not_always_true_or_false[0],
			#			make_bitmask(make_mask_from_indices(always_true))
			#		),
			#		file=output_file
			#	)

			# switch(selector)
			else:
				print("{}TOML_ASSUME_CODEPOINT_BETWEEN({}, {});".format(indent, make_literal(self.first), make_literal(self.last)), file=output_file)
				print("{}switch ({})\n{}{{".format(indent, selector, indent), file=output_file)
				if (len(always_true) == 0 and len(always_false) == 0):
					for subchunk_index in range(len(self.subchunks)):
						self.print_subchunk(subchunk_index, output_file, level, indent)
					print("{}\tTOML_NO_DEFAULT_CASE;".format(indent), file=output_file)
				elif (len(always_true) > len(always_false)):
					for subchunk_index in range(len(self.subchunks)):
						if not self.subchunks[subchunk_index].always_returns_true():
							self.print_subchunk(subchunk_index, output_file, level, indent)
					print("{}\tdefault: return true;".format(indent), file=output_file)
				else:
					for subchunk_index in range(len(self.subchunks)):
						if not self.subchunks[subchunk_index].always_returns_false():
							self.print_subchunk(subchunk_index, output_file, level, indent)
					print("{}\tdefault: return false;".format(indent), file=output_file)
				print("{}}}".format(indent), file=output_file)
			print("{}// chunk summary: {} codepoints from {} ranges (spanning a search area of {})".format(indent, self.count, len(self.ranges), self.span_size), file=output_file)

		# return cp == A || cp == B ...
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
				if ((line_weight + range_weight) > (4 - (1 if first_line else 0))):
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



#### FUNCTION GENERATOR #####################################



def emit_function(name, categories, file, codepoints):

	# divide the codepoints up into chunks of ranges
	root_chunk = Chunk(codepoints[0][0], codepoints[-1][0])
	first_codepoint = -1
	last_codepoint = -1
	for codepoint, category in codepoints:
		if (category in categories):
			if (first_codepoint == -1):
				first_codepoint = codepoint
				last_codepoint = codepoint
			elif (last_codepoint == codepoint-1):
				last_codepoint = codepoint
			else:
				root_chunk.add(first_codepoint, last_codepoint)
				first_codepoint = codepoint
				last_codepoint = codepoint
	if (first_codepoint != -1):
		root_chunk.add(first_codepoint, last_codepoint)
	root_chunk.trim()
	root_chunk.subdivide()

	# write the function

	print('\n\t/// \\brief Returns true if a codepoint belongs to any of these categories: {}'.format(', '.join(categories)), file=file)
	print('\t[[nodiscard]]', file=file)
	print('\tconstexpr bool {}(char32_t codepoint) noexcept\n\t{{'.format(name), file=file)
	root_chunk.print(file)
	print('\t}', file=file)



#### MAIN ####################################################



def get_script_folder():
    return os.path.dirname(os.path.realpath(sys.argv[0]))



def main():

	# get unicode character database
	codepoint_list = ''
	codepoint_file_path = os.path.join(get_script_folder(), 'UnicodeData.txt')
	if (not os.path.exists(codepoint_file_path)):
		print("Couldn't find unicode database file, will download")
		response = requests.get(
			'https://www.unicode.org/Public/UCD/latest/ucd/UnicodeData.txt',
			timeout=1
		)
		codepoint_list = response.text
		codepoint_file = open(codepoint_file_path,'w') 
		print(codepoint_list, end='', file=codepoint_file)
		codepoint_file.close()
	else:
		print("Reading unicode database file into memory")
		codepoint_file = open(codepoint_file_path,'r')
		codepoint_list = codepoint_file.read()
		codepoint_file.close()

	# parse the database file into codepoints
	re_codepoint = re.compile(r'^([0-9a-fA-F]+);(.+?);([a-zA-Z]+);')
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
				codepoints.append((cp, match.group(3)))
			current_range_start = -1
		else:
			if (match.group(2).endswith(', First>')):
				current_range_start = codepoint
			else:
				codepoints.append((codepoint, match.group(3)))
	print("Parsed {} codepoints from unicode database file.".format(len(codepoints)))
	codepoints.sort(key=lambda r:r[0])

	# write the output file
	output_file_path = os.path.join(get_script_folder(), '../include/toml++/toml_utf8_is_unicode_letter.h')
	print("Writing to {}".format(output_file_path))
	output_file = open(output_file_path,'w') 
	print('//# this file was generated by generate_is_unicode_letter.py', file=output_file)
	print('#pragma once', file=output_file)
	print('#include "toml_common.h"', file=output_file)
	print('\n#define TOML_ASSUME_CODEPOINT_BETWEEN(first, last)\t\\\n\tTOML_ASSUME(codepoint >= first);\t\t\t\t\\\n\tTOML_ASSUME(codepoint <= last)', file=output_file)
	print('\nnamespace toml::impl\n{', file=output_file, end='')
	emit_function('is_unicode_letter', ('Ll', 'Lm', 'Lo', 'Lt', 'Lu'), output_file, codepoints)
	emit_function('is_unicode_number', ('Nd', 'Nl'), output_file, codepoints)
	emit_function('is_unicode_combining_mark', ('Mn', 'Mc'), output_file, codepoints)
	print('}\n\n#undef TOML_ASSUME_CODEPOINT_BETWEEN', file=output_file)
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
		traceback.print_exc(file=sys.stderr)
		sys.exit(1)
	sys.exit()
