#include <sstream>
#include <list>
#include <fstream>

#include <string>
#include <map>

#include "ncwidgets.hpp"
#include "ncpalette.hpp"
#include "ncutil.hpp" // box_and_label()

namespace ncutil {

	NCDialog::NCDialog( int rows, int cols ) :
		NCPanel( rows, cols ), m_ch(0)
	{
 		this->bottom();
		this->bkgd( ' ' | color_pair("yellow","blue") | A_BOLD );
		this->scrollok( true );
	}


	NCWindow * NCDialog::getViewWidget()
	{
		if( ! this->m_ch )
		{
			this->m_ch = new NCWindow(*this,this->height()-2,this->width()-2,1,1);
			this->m_ch->scrollok(true);
		}
		return this->m_ch;
	}
	void NCDialog::decorate()
	{
		box_and_label( *this, this->title() );
		this->getViewWidget()->bkgd( ' ' | color_pair("black","white") );
		this->attron( A_BLINK );
		this->addstr( this->height()-1, 1, "(key?)" );
		this->attroff( A_BLINK );
	}


	NCDialog::~NCDialog()
	{
		this->clear();
		delete this->m_ch;
	}

	int NCDialog::doPopup()
	{
		this->getViewWidget()->addstr( 0, 0, this->text().c_str() );
		this->refresh();
		return ::getch();
	}

	std::string NCDialog::title() const
	{
		return this->m_title;
	}

	void NCDialog::title(const std::string & t )
	{
		this->m_title = t;
	}

	std::string NCDialog::text() const
	{
		return this->m_text;
	}

	void NCDialog::text(const std::string & t )
	{
		this->m_text = t;
	}


	int NCDialog::popup() throw(NCException)
	{
		this->decorate();
		this->top();
		this->show();
		int key = this->doPopup();
		this->hide();
 		this->refresh();
		return key;
	}

	int popup_dialog( const std::string & title,
			  const std::string & text,
			  int rows, int cols,
			  int y, int x ) throw(NCException)
	{
		/**
		   We need the sentry here because the call to screen_width()
		   might be confused about curses state if we've entered/left
		   it before. If we didn't use screen_width() then the NCDialog's
		   built-in sentry would suffice.
		*/
		NCMode sentry;
		int slen = std::max( title.size(), text.size() );
		int sw = screen_width()/2;
		if( slen-2 < sw ) sw = slen+2;
		if( -1 == cols )
		{
			cols = sw;
		}
		if( -1 == rows )
		{
			rows = (slen / sw) + 3;
		}

		NCDialog dlg(rows+2,cols+2);
		dlg.title( title );
		dlg.text( text );
		if( -1 == y || -1 == x )
		{
			center_window( dlg );
		} else
		{
			dlg.mvwin( y, x );
		}
		return dlg.popup();
	}







#define NCSTREAMPAD_BUFFSIZE(X) (X ? X : 400)
	NCStreamPad::NCStreamPad( NCWindow &towrap,
				  std::ostream & os,
				  int linebuffer,
				  int cols,
				  unsigned long curses_attr
				  ) throw(NCException)
		:  NCFramedPad(towrap,
			  NCSTREAMPAD_BUFFSIZE(linebuffer),
			  cols ? cols : (towrap.width()-2),
			  1, 1 ),
		   m_os(&os),
		   m_buf(0),
		   m_ownos(false)
	{
		this->init( linebuffer, curses_attr );
	}


	std::ostream &
	NCStreamPad::ostream()
	{
		return *(this->m_os);
	}

	NCStreamPad::NCStreamPad( NCWindow &towrap,
				  int linebuffer,
				  int cols,
				  unsigned long curses_attr
				  ) throw(NCException)
		:  NCFramedPad(towrap,
			  NCSTREAMPAD_BUFFSIZE(linebuffer),
			  cols ? cols : (towrap.width()-2),
			  1, 1 ),
		   m_os(new std::ostringstream),
		   m_buf(0),
		   m_ownos(true)
	{
// 		this->m_oldbuf = this->m_os->rdbuf();
		this->init( linebuffer, curses_attr );
	}

	void NCStreamPad::init( int linebuffer, unsigned long curses_attr )
	{
		linebuffer = NCSTREAMPAD_BUFFSIZE(linebuffer); // make sure
		this->m_buf = new NCStreamBuffer( *this, *this->m_os, curses_attr );
		/***
		    massive kludge:
		
		    pre-scroll to bottom, to make scrolling start with
		    the first screen of output, so user isn't forced
		    to manually...
		*/
		int at = 0;
  		for( ; at < linebuffer;
		     at++ )
  		{
			if( NCFramedPad::PadReqIgnored == this->requestOp( NCFramedPad::PadReqDown ) ) break;
			// ^^^^ presumably because of Out-o-bounds, so we'll stop scrolling
 		}
		this->inch( linebuffer - 2, 0 );
	}
#undef NCSTREAMPAD_BUFFSIZE


	NCStreamPad::~NCStreamPad()
	{
		this->clear();
		delete this->m_buf; // will reset m_os's buffer, if needed
		if( this->m_ownos ) delete this->m_os;
	}

	std::streambuf * NCStreamPad::rdbuf()
	{
		return this->m_buf;
	}


	NCFileViewer::NCFileViewer( int lines, int cols, int aty, int atx )
		throw(NCException)
		: NCPanel(lines,cols,aty,atx),
		  m_fname(""),
		  m_pad(0)
	{

	}

	NCFileViewer::NCFileViewer( const NCWindow & geom )
		throw(NCException)
		: NCPanel(geom.height(), geom.width()),
		  m_fname(""),
		  m_pad(0)
	{
		int y, x;
		geom.getyx( y, x );
		this->mvwin( y, x );
		this->bkgd( geom.getbkgd() );
	}

	NCFileViewer::~NCFileViewer()
	{
		this->clear();
		delete this->m_pad;
	}

	int NCFileViewer::noutrefresh()
	{
		this->drawLabels();
		this->touchwin();
		return this->NCPanel::noutrefresh();
	}

	int NCFileViewer::clear()
	{
		this->m_fname = "";
		delete this->m_pad;
		this->m_pad = 0;
		return this->NCPanel::clear();
	}
	void NCFileViewer::drawLabels()
	{
		if( this->m_pad && !this->m_fname.empty())
		{
			this->inch( 0, 2 );
			this->printw( 0, 2, " %s : %d / %d ",
				      this->m_fname.c_str(),
				      1+this->m_pad->topViewLine(),
				      this->m_pad->lineCount());
			int atr, atc;
			this->getyx( atr, atc );
			for( int i = atc; i < this->width()-1; i++ ) // clean up old numbers
			{
				this->addch( ACS_HLINE );
			}
		} else
		{
			this->box();
			this->printw( 0, 2, "No file loaded." ); 
		}
	}

	void NCFileViewer::doInputLoop()
	{
		this->refresh();
		if( ! this->m_pad )
		{
			popup_dialog( "NCFileViewer::doInputLoop()",
				      "No file loaded." );
			return;
		}
		this->m_pad->inputLoop();
	}

	bool NCFileViewer::load( std::istream & is,
				       bool dialogOnError ) throw(NCException)
	{
		if( ! is.good() )
		{
			if( dialogOnError ) popup_dialog( "NCFileViewer::load(stream)",
							  "(Input stream).good() == false" );
			return false;
		}
		typedef std::list<std::string> SList;
		SList lines;
		std::string line;
		int rows = 0;
		size_t cols = 0;
		while( std::getline( is, line ).good() )
		{
			++rows;
			// i am getting the WRONG max width here, dammit!
			// my input has 96 cols and it's saying 82 is higher!
// 			cols = std::max( cols, line.size() );
			lines.push_back( line );
		}
		if( 0 == rows )
		{
			if( dialogOnError ) popup_dialog( "NCFileViewer::load(stream)",
							  "Input stream was empty." );
			return false;
		}
 		//cols = std::max( (int)cols, this->width()-2 ); // pad requires a min width
		cols = 200; // if we allow line wrapping then the top of the file may scroll away!
		this->clear();
		this->m_fname = "<stream>";
		this->m_pad = new NCStreamPad( *this, rows, cols );
		this->m_pad->bkgd( this->getbkgd() );
		this->m_pad->inch( 0, 0 );
		SList::const_iterator it = lines.begin();
		std::ostream & os = this->m_pad->ostream();
		for( ; lines.end() != it; ++it )
		{
			os << (*it) << "\n";
		}
		// os << "COLS="<<cols<<"\n";
		return true;
	}

	bool NCFileViewer::load( const std::string & fname,
				     bool dialogOnError ) throw(NCException)
	{
		std::ifstream ifs(fname.c_str());
		if( ! ifs.good() )
		{
			if( dialogOnError ) popup_dialog( "NCFileViewer::load(string)",
							  "Could not open file:\n"+fname );
			return false;
		}
		bool ret = load( ifs, dialogOnError );
		if( ret )
		{ // otherwise pad didn't change
			this->m_fname = fname;
			this->drawLabels();
			this->refresh();
		}
		return ret;
	}

				

} // namespace ncutil
