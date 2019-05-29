#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim:ts=4:sw=4:expandtab

# GenErrTable.py
#
# Copyright (c) 1999-2017, Parallels International GmbH
# Copyright (c) 2017-2019 Virtuozzo International GmbH. All rights reserved.
#
# This file is part of Virtuozzo SDK. Virtuozzo SDK is free
# software; you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 2.1 of the License,
# or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library.  If not, see
# <http://www.gnu.org/licenses/>.
#
# Our contact details: Virtuozzo International GmbH, Vordergasse 59, 8200
# Schaffhausen, Switzerland; http://www.virtuozzo.com/.
#

import os, sys, string, xml
from xml.etree import ElementTree
et = ElementTree

# Full path of this python file module
__full_file_path__ = os.path.split( os.path.abspath(__file__) )[0]
if __full_file_path__ == '':
	__full_file_path__ = os.path.abspath( os.path.getcwd() )

ERR_INCLUDE_PATH = os.path.join(__full_file_path__, 'Include')
ERR_STR_STORAGE_PATH = os.path.join(__full_file_path__, 'Handles')


def read_file(file_name, path=''):
    file_name = path and os.path.join(path, file_name) or file_name
    return open(file_name).read()

def parse_error_codes():
	data = read_file('PrlErrorsValues.h', ERR_INCLUDE_PATH)
	data += read_file('PrlPrivateErrorsValues.h', ERR_INCLUDE_PATH)
	lines = data.splitlines()

	dict_errors = {}
	for line in lines:
		row = line.split()
		if len(row) < 3 or row[0] != '#define':
			continue
		err_str = row[1]
		err_code = row[2]
		if err_code in dict_errors:
			dict_errors[err_str] = dict_errors[err_code]
		else:
			dict_errors[err_str] = err_code.strip('PRLESUTDCAO_()')
	return dict_errors

def parse_err_strings_storage(pt, dict_errors):
	data = read_file('PrlErrStringsStorage.cpp', ERR_STR_STORAGE_PATH)
	data += read_file('PrlErrStringsStorage_' + pt.upper() + '.cpp', ERR_STR_STORAGE_PATH)

	raw_txt_list = data.split('REGISTER_ERROR_STRING')

	table = []
	for txt in raw_txt_list:
		tl = txt.splitlines()

		for es in tl:
			if es.find('_') != -1:
				err_str = es
				break
		err_str = err_str.strip().strip('2(,').strip()
		if err_str not in dict_errors:
			continue

		for es in tl:
			if es.find('PMT_') != -1:
				err_sign = es
				break
		err_sign = err_sign.split(')')[0].strip()

		err_data = [err_str, dict_errors[err_str], err_sign]
		for i in range(1, len(tl) - 2):
			str = tl[i].lstrip().rstrip()
			if not str.startswith('tr'):
				continue
			str = str.lstrip('tr(').rstrip('),').lstrip().rstrip()
			str = str[1:len(str)-1]
			str = str.replace('\"', '"')
			err_data.append(str)
		table.append(err_data)
	return table

def gen_xml_doc(table):
	tb = ElementTree.TreeBuilder()
	tb.start('messages', {})

	for row in table:
		tb.start('item', {})

		tb.start('Name', {})
		tb.data(row[0])
		tb.end('Name')

		tb.start('Code', {})
		tb.data(row[1])
		tb.end('Code')

		tb.start('Status', {})
		tb.data(row[2])
		tb.end('Status')

		tb.start('Text', {})
		if len(row) >= 4:
			tb.data(row[3])
		tb.end('Text')

		tb.start('NeedsKB', {})
		tb.end('NeedsKB')

		tb.start('Description', {})
		tb.end('Description')

		tb.end('item')

		for i in range(4, len(row)):
			tb.start('item', {})

			tb.start('Text', {})
			tb.data(row[i])
			tb.end('Text')

			tb.end('item')

	tb.end('messages')
	root_el = tb.close()

	et = ElementTree.ElementTree(root_el)
	et.write( os.path.join(__full_file_path__, 'ErrorsTable.xml') )

def gen_table(pt):
	print 'Parsing error codes...'
	dict_errors = parse_error_codes()

	print 'Parsing error strings storage...'
	table = parse_err_strings_storage(pt, dict_errors)

	print 'Generate XML document...'
	gen_xml_doc(table)

	print 'Done'

	return 0


def main():
	try:
		if len(sys.argv) < 2:
			print 'Usage: GenErrTable.py <pd | pw | ps>'
			return 2
		pt  = sys.argv[1]
		if pt != 'pd' and pt != 'pw' and pt != 'ps':
			print 'Unknown product type'
			return 3
		return gen_table(pt)
	except Exception, e:
		print e
		return 1
	return 0

if  __name__ == '__main__':
	sys.exit(main())
