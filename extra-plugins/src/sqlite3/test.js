loadPlugin('v8-juice-sqlite3');
var my = {db:0,stmt:0};
my.db = sqlite3_open("my.db");
print("Using sqlite3 version",sqlite3_libversion());

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

if(0)
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

if(0)
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

if(0) // crashing in weird places, seems my Function handles are disappearing again :(
{
    // This function will be called once per row in a result set:
    function step_func( cx, userData, values ) {
	print("step_func()");
	// Here we simply accumulate all of the column counts:
	userData.counter += values.length;
	// Ideally you want want some algorithm which does
	// something useful!
	return SQLITE_OK;
    }
    
    // This function will be called once at the end of the result set:
    function final_func( cx, userData ) {
	// Write grand total and reset the counter for the next call:
	print("final_func()");
	userData.total = userData.counter;
	userData.counter = 0;
	sqlite3_result_int( cx, userdata.total );
	return SQLITE_OK;
    }
    // Now register the functions with sqlite3:
    var rc = sqlite3_create_function(
				     my.db, 			   // db handle
				     'myAccum',           // function name
				     -1,                  // argument count, or -1 for "any number"
				     SQLITE_UTF8,         // encoding
				     {counter:0,total:0}, // userData param passed to step_func/final_func
				     null,                // this is always null for aggregator functions
				     step_func,           // reference to the 'step' function
				     final_func           // reference to the 'final' func
				     );
    print('myAccum(*) =',sqlite3_select_text( my.db, "select myAccum(*) from t" ));
}

print("Shuting down...");
sqlite3_close( my.db );
print("Done!");
