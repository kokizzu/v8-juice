#!/usr/bin/perl
########################################################################
# A quick hack to generate C++ class templates for use with the
# parsepp library.
#
# Usage:
#
#  $0 NUMBER (default=10)
#
# It generates master templates with NUMBER arguments and specializations
# for N arguments, where (0 > N < NUMBER).
#
# The generated classes are:
#
#  TypeList_N<N types>
#
#  TypeList<N types>
########################################################################
my $max = @ARGV ? $ARGV[0] : 10;

if( $max !~ /^\d+$/ ) {
    die "Usage: $0 NUMBER\n";
}
print "// auto-generated [".localtime()."] by $0\n";
for( my $i = 0; $i <= $max; ++$i ) {

    if( 0 == $i ) {
	print "struct TypeList_0 : TypeListBase<NilType,NilType> {};\n";
	print "\n";
	next;
    }
    if( 1 == $i ) {
	print "template <class T1>\n";
	print "struct TypeList_1 : TypeListBase<T1,NilType> {};\n";
	print "\n";
	next;
    }
    if( 2 == $i ) {
	print "template <class T1, class T2>\n";
	print "struct TypeList_2 : TypeListBase<T1, TypeListBase<T2, NilType> > {};\n";
	print "\n";
	next;
    }
    my @tlist;
    my @vlist;
    for( my $x = 2; $x <= $i; ++$x ) {
	push(@tlist, 'T'.$x );
	push(@vlist, 'V'.$x );
    }
    my $Tlist = join(', ',@tlist);
    #my $TPlist = '('.join('), (',@tlist).')';

    my $typenames = 'class '.join(', class ',@tlist);
    print "template <class T1, ".$typenames." >\n";
    my $parent = "TypeListBase<T1, TypeList_".($i-1)."< ".$Tlist." > >";
    print "struct TypeList_".$i." : ".$parent." {};\n";
    print "\n";

}

my $defaultInt = '-1';
for( my $i = $max; $i >= 0; --$i ) {
    my @nilfiller;
    my @zerofiller;
    for( my $y = ($max-$i); $y > 0; --$y ) {
	my $kludge = -1;
	push( @nilfiller, "NilType" );
	push( @zerofiller, $defaultInt );
    }
    my $nils = join(', ',@nilfiller );
    if( @nilfiller ) { $nils = ', '.$nils; }
    my $zeros = join(', ',@zerofiller );
    if( @zerofiller ) { $zeros = ', '.$zeros; }

    if( 0 == $i ) {
	print "template <>\n";
	print "struct TypeList<> : TypeListBase<NilType,NilType> {};\n";
	print "\n";
	next;
    }
    if( 1 == $i ) {
	print "template <class T1>\n";
	print "struct TypeList<T1".$nils."> : TypeList_1<T1> {};\n";
	print "\n";
	next;
    }
    if( 2 == $i ) {
	print "template <class T1, class T2>\n";
	print "struct TypeList<T1, T2".$nils."> : TypeList_2<T1,T2> {};\n";
	print "\n";
	next;
    }
    my @tlist;
    my @vlist;
    for( my $x = 2; $x <= $i; ++$x ) {
	push(@tlist, 'T'.$x );
	push(@vlist, 'V'.$x );
    }
    my $Tlist = join(', ',@tlist);
    my $Vlist = join(', ',@vlist);

    my $typenames = 'class '.join(', class ',@tlist);

    my @tdecllistDefaults;
    my @tdecllist;
    my @nilfiller;
    my @zerofiller;
    for( my $y = 2; $y <=$i; ++$y ) {
	push( @tdecllistDefaults, "class T".$y." = NilType" );
	push( @tdecllist, "class T".$y );
    }
    my $tdecllistDefaults = join(', ',@tdecllistDefaults);
    my $tdecllist = join(', ',@tdecllist);
    if( $max != $i ){ # Specialization
	print "template <class T1, ".$tdecllist.">\n";
	print "struct TypeList <T1, ".$Tlist.$nils."> : TypeList_".$i."<T1, ".$Tlist."> {};\n";
    } else { # Master template
	print "template <class T1 = NilType, ".$tdecllistDefaults.">\n";
	print "struct TypeList : TypeList_".$i."<T1, ".$Tlist."> {};\n";
    }
    print "\n";
}
