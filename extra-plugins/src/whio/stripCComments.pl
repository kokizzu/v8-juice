#!/usr/bin/env perl
# Strips C-style comments.
# Source: http://www.perl.com/doc/FAQs/FAQ/oldfaq-html/Q4.27.html
$/ = undef;
$_ = <>; 

s#/\*[^*]*\*+([^/*][^*]*\*+)*/|([^/"']*("[^"\\]*(\\[\d\D][^"\\]*)*"[^/"']*|'[^'\\]*(\\[\d\D][^'\\]*)*'[^/"']*|/+[^*/][^/"']*)*)#$2#g;
print;

