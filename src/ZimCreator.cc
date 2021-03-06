#include <string>
#include <iostream>
#include <zim/writer/zimcreator.h>

#include "Article.cc"
#include "nbind/api.h"

class OverriddenZimCreator : public zim::writer::ZimCreator
{
  public:
	OverriddenZimCreator(std::string mainPage)
		: zim::writer::ZimCreator(true),
		  mainPage(mainPage) {}

	virtual std::string getMainPage()
	{
		return mainPage;
	}

	std::string mainPage;
};

class ZimCreatorWrapper
{
  public:
	ZimCreatorWrapper()
	{
	}

	OverriddenZimCreator *_c;
};

class ZimCreatorManager
{
  public:
	static ZimCreatorWrapper *create(std::string fileName, std::string mainPage, std::string fullTextIndexLanguage, int minChunkSize)
	{
		bool shouldIndex = !fullTextIndexLanguage.empty();

		static ZimCreatorWrapper cw;
		cw._c = new OverriddenZimCreator(mainPage); // TODO: consider when to delete this
		cw._c->setIndexing(shouldIndex, fullTextIndexLanguage);
		cw._c->setMinChunkSize(minChunkSize);
		cw._c->startZimCreation(fileName);
		return (&cw);
	}

	static void addArticle(ZimCreatorWrapper *cw, Article article, nbind::cbFunction &callback)
	{
		try
		{
			cw->_c->addArticle(article.toZimArticle());
			callback();
		}
		catch (...)
		{
			callback("Failed to add article");
			throw;
		}
	}

	static void finalise(ZimCreatorWrapper *cw, nbind::cbFunction &callback)
	{
		cw->_c->finishZimCreation();
		delete cw->_c;
		callback();
	}
};

#include "nbind/nbind.h"

#ifdef NBIND_CLASS

NBIND_CLASS(ZimCreatorWrapper)
{

	construct();
}

NBIND_CLASS(ZimCreatorManager)
{
	method(create);
	method(addArticle);
	method(finalise);
}

#endif
