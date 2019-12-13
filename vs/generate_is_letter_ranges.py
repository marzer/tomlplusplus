#!/usr/bin/env python3

import sys
import re
import math
import requests
import traceback

def make_literal(codepoint):
	if (codepoint > 0xFFFF):
		return "U'\\U{:08X}'".format(codepoint)
	else:
		return "U'\\u{:04X}'".format(codepoint)

class Chunk:

	def __init__(self, first, last):
		self.first = int(first)
		self.last = int(last)
		self.span_size = self.last - self.first + 1
		self.count = 0
		self.ranges = []
		self.subchunks = None
		self.subchunk_div = 0

	def add(self, first, last = None):
		if (last is None or first == last):
			self.ranges.append(int(first))
			self.count += 1
		else:
			self.ranges.append((int(first), int(last)))
			self.count += (self.ranges[-1][1] - self.ranges[-1][0] + 1)

	def subdivide(self, subchunk_count = -1):
		if (self.subchunks is not None or self.count == self.span_size):
			return
		if (subchunk_count <= 0):
			subchunk_count = int(len(self.ranges) / 5)
		if (subchunk_count <= 1 or len(self.ranges) <=subchunk_count):
			return

		self.subchunks = []
		self.subchunk_div = int(subchunk_count)
		subchunk_size = int(math.ceil((self.last - self.first + 1) / float(self.subchunk_div)))
		for subchunk in range(subchunk_count):
			self.subchunks.append((
				subchunk,
				Chunk(
					self.first + (subchunk * subchunk_size),
					min(self.first + (((subchunk + 1) * subchunk_size) - 1), self.last)
				)
			))
		for r in self.ranges:
			if (isinstance(r, int)):
				subchunk = int((r - self.first) / subchunk_size)
				self.subchunks[subchunk][1].add(r)
			else:
				start_chunk = int((r[0] - self.first) / subchunk_size)
				end_chunk = int((r[1] - self.first) / subchunk_size)
				for subchunk in range(start_chunk, end_chunk+1):
					self.subchunks[subchunk][1].add(
						max(r[0], self.subchunks[subchunk][1].first),
						min(r[1], self.subchunks[subchunk][1].last),
					)
		self.ranges = None
		newsubchunks = []
		for subchunk in self.subchunks:
			if subchunk[1].first != -1:
				subchunk[1].subdivide()
				newsubchunks.append(subchunk)
		self.subchunks = newsubchunks

	def print(self, level = 0):
		indent = '\t' * (2 * level)
		if (self.count == self.span_size):
			print("return true;")
		elif (self.subchunks is not None):
			print("{}switch ((static_cast<uint32_t>(codepoint) - 0x{:X}u) / {}u)\n{}{{".format(
				indent,
				self.first,
				self.subchunk_div,
				indent
			))
			for subchunk_index, subchunk in self.subchunks:
				if (subchunk.count == 0):
					continue
				print("{}\tcase {}: ".format(indent, subchunk_index), end='')
				if (subchunk.count == subchunk.span_size or subchunk.subchunks is None):
					subchunk.print(level + 1)
				else:
					print("\n{}\t{{".format(indent))
					subchunk.print(level + 1)
					print("{}\t}}".format(indent))
			print("{}}}".format(indent))
			print("{}// total codepoints in chunk: {}\n".format(
				indent,
				self.count
			))
		else:
			print("return ", end='')
			for range_idx in range(0, len(self.ranges)):
				if (range_idx > 0):
					print("\n\t{}|| ".format(indent), end='')
				r = self.ranges[range_idx]
				if (isinstance(r, int)):
					print("codepoint == {}".format(make_literal(r)), end='')
				else:
					print("(codepoint >= {} && codepoint <= {})".format(make_literal(r[0]), make_literal(r[1])), end='')
			print(";")
			print("\t{}// total codepoints in chunk: {}\n".format(
				indent,
				self.count
			))

def main():

	# get unicode data
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
	response = requests.get(
		'https://www.unicode.org/Public/UCD/latest/ucd/UnicodeData.txt',
		timeout=1
	)
	codepoint_list = response.text
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
	root_chunk.subdivide(30)
	
	# emit the code to copy and paste into is_letter()
	print("\n// this code was generated by a script;\n// see /vs/generate_is_letter_ranges.py\n")
	print("if (codepoint < {} || codepoint > {})\n\treturn false;\n".format(
		make_literal(codepoints[0]),
		make_literal(codepoints[-1])
	))
	root_chunk.print()

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
