load_plugin('v8-juice-sqlite3');
var my = {db:0,stmt:0};
my.db = sqlite3_open("my.db");
print("Using sqlite3 version",sqlite3_libversion());


/**
print(my.db); sefaults:

Program received signal SIGABRT, Aborted.
[Switching to Thread 0xb7d566c0 (LWP 12233)]
0xb8097430 in __kernel_vsyscall ()
(gdb) bt
#0  0xb8097430 in __kernel_vsyscall ()
#1  0xb7d838a0 in raise () from /lib/tls/i686/cmov/libc.so.6
#2  0xb7d85268 in abort () from /lib/tls/i686/cmov/libc.so.6
#3  0x080fc995 in v8::internal::OS::Abort () at src/platform-linux.cc:277
#4  0x08071b66 in V8_Fatal (file=0x81a89ee "src/handles-inl.h", line=48, 
    format=0x81a890a "CHECK(%s) failed") at src/checks.cc:58
#5  0x08052c5f in CheckHelper (file=0x81a89ee "src/handles-inl.h", line=48, 
    source=0x81a8a00 "reinterpret_cast<Address>(*location_) != kHandleZapValue", condition=false)
    at src/checks.h:62
#6  0x0805f4c3 in v8::internal::Handle<v8::internal::Object>::operator* (this=0xbf898f10)
    at src/handles-inl.h:48
#7  0x0805f4dd in v8::internal::Handle<v8::internal::Object>::operator-> (this=0xbf898f10)
    at src/handles.h:61
#8  0x080569e8 in v8::Value::ToString (this=0x8916980) at src/api.cc:1347
#9  0x0805a3aa in AsciiValue (this=0xbf898f94, obj={val_ = 0x8916980}) at src/api.cc:2775
#10 0x0804ddc4 in v8::convert::Detail::to_native_f<std::string>::operator() (this=0xbf898fd3, 
    h=@0xbf899038) at v8-convert.h:230
#11 0x0804de9a in v8::convert::CastFromJS<std::string> (h=@0xbf899038) at v8-convert.h:243
#12 0x0804c98f in main (argc=2, argv=0xbf899184) at my.cc:183
*/

my.stmt = sqlite3_prepare( my.db, "create table if not exists t(a,b,c)");
print('typeof stmt ==',typeof my.stmt,', errmsg =',sqlite3_errmsg(my.db));
var rc = sqlite3_step( my.stmt );
print("step() rc ==",rc);
rc = sqlite3_finalize( my.stmt );
my.stmt = 0;
print("finalize() rc ==",rc);


if(1)
{
    my.stmt = sqlite3_prepare( my.db, "insert into t(a,b,c) values (?,?,?)" );
    var now = (new Date()).toString();
    for( var i = 0; i < 5; ++i )
    {
	sqlite3_bind( my.stmt, 1, 42 + i );
	sqlite3_bind( my.stmt, 2, 24.42 * i );
	//sqlite3_bind_text( my.stmt, 3, now+": #"+i, 10 );
	sqlite3_bind_text( my.stmt, 3, now+": #"+i );
	sqlite3_step( my.stmt );
	sqlite3_clear_bindings( my.stmt );
	sqlite3_reset( my.stmt );
    }
    sqlite3_finalize( my.stmt );
}

print("Entry count:",sqlite3_select_text(my.db,"select count(*) from t"));


if(1)
{
    function my_exec_cb(cbdata,rows,cols)
    {
	if( 1 == ++cbdata.pos )
	{
	    print("my_exec_cb()! cbdata =["+cbdata+"]",'rows =',typeof rows, rows.length,', cols =',typeof cols,cols.length);
	    print( cols.join('\t'));
	}
	print( rows.join('\t'));
	if( 0 && (cbdata.pos > 3) )
	{
	    throw new Error("Throwing from my_exec_cb()!");
	}
	return SQLITE_OK;
    }
    //rc = sqlite3_exec(my.db,"select myfunc('hi','world')",my_exec_cb);
    rc = sqlite3_exec(my.db,"select rowid,* from t order by rowid desc limit 7",my_exec_cb,{pos:0});
    print("exec() rc=",rc);
}

if( 1 )
{
    print("sqlite3_create_function() test...");
    function my_cb( sqcx, data, values )
    {
	print( "my_cb()!!! argc =",arguments.length, ", values.length =",values.length);//,"(",values.join(','),")" );
	for( var i = 0; i < values.length; ++i )
	{
	    print( "sqlite3_value_text(value #"+i+") =",sqlite3_value_text(values[i]));
	}
	sqlite3_result_value( sqcx, values[0] );
	return SQLITE_OK;
    }

    rc = sqlite3_create_function( my.db, "myfunc", -1, SQLITE_UTF8, {msg:"hi, callback"}, my_cb );
    print("create_function() rc=",rc);
    rc = sqlite3_select_int(my.db,"select myfunc(7,11,13)");
    print("myfunc() rc=",rc,', errmsg=',sqlite3_errmsg(my.db));
}

if(0)
{
    var str = "i'm stephan, who're you?";
    print("str:", str );
    sqlite3_sleep( 1000 );
    print("esc'd:", sqlite3_escape_sql( str ) );
    sqlite3_sleep( 1000 );
    print("quoted:", sqlite3_quote_sql( str ) );
    sqlite3_sleep( 1000 );
}

if(1)
{
    print("js_eval() test...");
    sqlite3_exec( my.db, "select js_eval('sqlite3_select_int(my.db,''select count(*) from t'')')");
    print( 'js_eval():',
	   sqlite3_select_text( my.db,
				"select js_eval('print(''hello, world'');''hi there'';')"
				)
	   );
    print( 'js_eval():',
	   sqlite3_select_text( my.db,
				"select js_eval('throw new Error(''testing throwing from js_eval!'')')"
				)
	   );
    print( 'js_eval():',
	   sqlite3_select_text( my.db,
				"select js_eval()"
				)
	   );
    print("js_eval() tests done.");
}

if(1)
{
    var ar = sqlite3_select_array( my.db, "select * from t order by rowid desc limit 7" );
    print('select array:',ar);
    var sep = '\t';
    if( ar instanceof Object )
    {
	for( var r = 0; r < ar.rows.length; ++r )
	{
	    if( 0 == r )
	    {
		print(ar.cols.join(sep));
	    }
	    print( ar.rows[r].join(sep) );
	}
	//print('select cols:',ar.cols);
	//print('select rows:',ar.rows);
    }
}

print("Shuting down...");
sqlite3_close( my.db );
print("Done!");
