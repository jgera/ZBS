#!/usr/bin/python

from os import *
from string import *


if __name__ == "__main__":
	passed = []

	for i in range(200, 1600, 1):
		r = '%f' % (i/10000.)	
		if system('./p '+r+' 700') == 0:
			passed.append(r)

	print passed

# result:
#'0.022800', '0.022900', '0.023000', '0.023100', '0.023200', '0.023300', '0.023400', 
#'0.023500', '0.023600', '0.023700', '0.023800', '0.023900', '0.024000', '0.024100',
#'0.024200', '0.024300', '0.024400', '0.024500', '0.024600', '0.024700', '0.024800', 
#'0.024900', '0.025000', '0.025100', '0.025200', '0.025300', '0.025400', '0.025500', 
#'0.025600', '0.025700', '0.025800', '0.025900', '0.026000', '0.026100', '0.026200', 
#'0.026300', '0.026400', '0.026500', '0.026600', '0.026700', '0.027400', '0.028100',
#'0.031600', '0.031700', '0.031800', '0.031900', '0.032000', '0.032100', '0.032200',
#'0.032300', '0.032400', '0.032500', '0.032600', '0.032700', '0.032800', '0.032900',
#'0.033000', '0.033100', '0.033200', '0.033300', '0.033400', '0.033500', '0.033600', 
#'0.033700', '0.033800', '0.033900', '0.034000', '0.034100', '0.034200', '0.034300',
#'0.034400', '0.034500', '0.034600', '0.050000', '0.050100', '0.050200', '0.050300',
#'0.050400', '0.050500', '0.050600'

# vim:ts=2:sw=2:
