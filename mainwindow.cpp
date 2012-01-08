#include "mainwindow.h"

#include <QtGui/QApplication>
#include <QtWebKit>
#include <QLineEdit>
#include <QToolbar>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlDriver>
#include <QIODevice>
#include <QTextStream>
#include <QTextCodec>
#include <QXmlStreamWriter>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    unrecoverableError=0;
    project=-1;
    approach=-1;

    if(!setupDatabase())
    {
	unrecoverableError=1;
	return;
    }

    view = new QWebView(this);
    view->load(QUrl("html/index.html"));
    connect(view->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(populateJavaScriptWindowObject()));
    setCentralWidget(view);

    this->setWindowTitle(tr("Historical Phonology Workplace 1.1.1 (written by Adam Baker; released under GPL)"));
}

MainWindow::~MainWindow()
{

}

void MainWindow::populateJavaScriptWindowObject()
{
    view->page()->mainFrame()->addToJavaScriptWindowObject("app", this);
}

void MainWindow::RefreshFromDatabase()
{
    QFile f("html/context.js");
    if (!f.open(QFile::WriteOnly | QFile::Text))
    {
	QMessageBox::warning(this, tr("Codecs"),tr("Can't open the javascipt output file to truncate it."));
	return;
    }
    f.close();
    // write in some necessary objects
    writeJavaScriptToFile("var app = Array();\n");
    writeJavaScriptToFile("top.app.RefreshFromDatabase = function RefreshFromDatabase() { return true; }\n");
    writeJavaScriptToFile("top.app.disassociateCognateCorrespondence = function disassociateCognateCorrespondence() { return true; }\n");
    writeJavaScriptToFile("top.app.associateCorrespondenceReconstruction = function associateCorrespondenceReconstruction() { return true; }\n");
    writeJavaScriptToFile("top.app.createCorrespondenceSet = function createCorrespondenceSet() { return true; }\n");
    writeJavaScriptToFile("top.app.setCookie = function setCookie() { return true; }\n");
    writeJavaScriptToFile("top.app.getCookies = function getCookies() { return ''; }\n");
    writeJavaScriptToFile("top.app.setProjectApproach = function setProjectApproach() { return true; }\n\n");

    // makes sense to bundle certain ones
    refreshProjectApproach();
    refreshItems();
    refreshLanguages();

    if(project!=-1 && approach!=-1)
    {
	javascript2DArrayFromQuery("select form,setid,langid from protoforms,languages where languages.id=protoforms.langid and languages.approach='"+QString::number(approach)+"';","top.protoforms","setid","langid","form");
	javascript1DArrayFromQuery("select * from lemmas where project='"+QString::number(project)+"';","top.lemmas","id","gloss");
	javascript2DArrayFromQueryArray("select setid,group_concat(lemmaid,',') as lemmaid,langid from cognate_sets,cognate_set_elements,items where items.id=cognate_set_elements.itemid and cognate_sets.id=cognate_set_elements.setid and cognate_sets.approach='"+QString::number(approach)+"' group by setid,langid;","top.item_cognate_dep","setid","langid","lemmaid");
	javascript2DArrayFromQuery("select cognate,correspondence from cognate_correspondence,cognate_sets where cognate_sets.approach='"+QString::number(approach)+"' and cognate_sets.id=cognate_correspondence.cognate;","top.cog_corr_dep","correspondence","cognate","cognate");
	javascript1DArrayFromQuery("select id,name from cognate_sets where approach='"+QString::number(approach)+"';","top.cognate_labels","id","name");
	javascript1DArrayFromQuery("select id,note from cognate_sets where approach='"+QString::number(approach)+"';","top.cognate_notes","id","note");
	javascript1DArrayFromQuery("select id,note from correspondence_sets where approach='"+QString::number(approach)+"';","top.correspondence_notes","id","note");
	javascript1DArrayFromQuery("select soundchanges.id,soundchanges.note from soundchanges,reconstructions where soundchanges.reconstruction=reconstructions.id and reconstructions.approach='"+QString::number(approach)+"';","top.soundchange_notes","id","note");
	javascript1DArrayFromQuery("select id,note from reconstructions where approach='"+QString::number(approach)+"';","top.reconstruction_notes","id","note");
	javascript2DArrayFromQuery("select group_concat(form,'<br/>') as form,cognate_set_elements.setid,items.langid from items,cognate_set_elements,cognate_sets where approach='"+QString::number(approach)+"' and cognate_set_elements.setid=cognate_sets.id and cognate_set_elements.itemid=items.id  group by setid,langid;","top.cognate_sets","setid","langid","form");
	javascript1DArrayFromQuery("select id,ifnull(count,0) as count from (select id from cognate_sets where approach='"+QString::number(approach)+"') as a left join (select cognate,count(*) as count from cognate_correspondence where cognate in (select id from cognate_sets where approach='"+QString::number(approach)+"') group by  cognate) as b on a.id=b.cognate;","top.cognate_correspondence_counts","id","count");
	javascript1DArrayFromQuery("select id,ifnull(count,0) as count from (select id from cognate_sets where approach='"+QString::number(approach)+"') as a left join (select cognate,count(*) as count from cognate_correspondence,correspondence_reconstruction where cognate_correspondence.correspondence=correspondence_reconstruction.correspondence group by cognate) as b on a.id=b.cognate;","top.cognate_reconstruction_counts","id","count");
	javascript2DArrayFromQuery("select * from correspondence_set_elements,correspondence_sets where correspondence_sets.id=correspondence_set_elements.setid and correspondence_sets.approach='"+QString::number(approach)+"';","top.correspondences","setid","langid","string");
	javascript1DArrayFromQuery("select id,ifnull(count,0) as count from (select id from correspondence_sets where approach='"+QString::number(approach)+"') as a left join (select correspondence,count(*) as count from cognate_correspondence where correspondence in (select id from correspondence_sets where approach='"+QString::number(approach)+"') group by correspondence) as b on a.id=b.correspondence;","top.correspondence_cognate_counts","id","count");
	javascript1DArrayFromQuery("select id,ifnull(count,0) as count from (select id from correspondence_sets where approach='"+QString::number(approach)+"') as a left join (select correspondence,count(*) as count from correspondence_reconstruction where reconstruction in (select id from reconstructions where approach='"+QString::number(approach)+"') group by correspondence) as b on a.id=b.correspondence;","top.correspondence_reconstruction_counts","id","count");
	javascript2DArrayFromQuery("select correspondence,reconstruction from correspondence_reconstruction,correspondence_sets where approach='"+QString::number(approach)+"';","top.cor_recon_dep","reconstruction","correspondence","correspondence");
	javascript1DArrayFromQuery("select y.id,ifnull(num_correspondences,0) as count from (select id from reconstructions where approach='"+QString::number(approach)+"') as y left join (select reconstructions.id,count(correspondence) as num_correspondences,reconstructions.string from reconstructions,correspondence_reconstruction where reconstructions.id=correspondence_reconstruction.reconstruction and reconstructions.approach='"+QString::number(approach)+"' group by reconstructions.id) as x on y.id=x.id;","top.recon_correspondences","id","count");
	javascript1DArrayFromQuery("select y.id,ifnull(num_cognates,0) as count from (select id from reconstructions where approach='"+QString::number(approach)+"') as y left join (select reconstructions.id,count(cognate) as num_cognates from reconstructions,correspondence_reconstruction,cognate_correspondence where reconstructions.id=correspondence_reconstruction.reconstruction and correspondence_reconstruction.correspondence=cognate_correspondence.correspondence and reconstructions.approach='"+QString::number(approach)+"' group by reconstructions.id) as x on y.id=x.id;","top.recon_cognates","id","count");
	javascript1DArrayFromQuery("select y.id,ifnull(num_languages,0) as count from (select id from reconstructions where approach='"+QString::number(approach)+"') as y left join (select id,count(langid)as num_languages from (select distinct reconstructions.id,items.langid from reconstructions,correspondence_reconstruction,cognate_correspondence,cognate_set_elements,items where reconstructions.id=correspondence_reconstruction.reconstruction and correspondence_reconstruction.correspondence=cognate_correspondence.correspondence and cognate_set_elements.setid=cognate_correspondence.cognate and items.id=cognate_set_elements.itemid and reconstructions.approach='"+QString::number(approach)+"') as a group by id) as x on y.id=x.id;","top.recon_languages","id","count");
	javascript1DArrayFromQuery("select y.id,ifnull(num_elements,0) as count from (select id from reconstructions where approach='"+QString::number(approach)+"') as y left join (select reconstructions.id,count(cognate_set_elements.itemid) as num_elements from reconstructions,correspondence_reconstruction,cognate_correspondence,cognate_set_elements where reconstructions.id=correspondence_reconstruction.reconstruction and correspondence_reconstruction.correspondence=cognate_correspondence.correspondence and cognate_set_elements.setid=cognate_correspondence.cognate and reconstructions.approach='"+QString::number(approach)+"' group by reconstructions.id) as x on y.id=x.id;","top.recon_items","id","count");
	javascriptFromQuery("select soundchanges.id,reconstruction,reconstructions.langid as sourcelanguage,follows,languages.fullname,reconstructions.string,soundchanges.becomes,soundchanges.before,soundchanges.after,soundchanges.inlanguage from soundchanges,reconstructions,languages where reconstructions.langid=languages.id and soundchanges.reconstruction=reconstructions.id and reconstructions.approach='"+QString::number(approach)+"' order by soundchanges.id;","top.soundchanges");
	javascript2DArrayFromQuery("select soundchange,cognate from soundchange_cognate,soundchanges,reconstructions where reconstructions.approach='"+QString::number(approach)+"' and soundchanges.id=soundchange_cognate.soundchange and soundchanges.reconstruction=reconstructions.id;","top.soundchange_cognate_dep","soundchange","cognate","cognate");
	javascript1DArrayFromQuery("select id,ifnull(count,0) as count from (select id from soundchanges) as a left join (select soundchange,count(cognate) as count from soundchange_cognate,soundchanges,reconstructions where reconstructions.approach='"+QString::number(approach)+"' and soundchanges.id=soundchange_cognate.soundchange and soundchanges.reconstruction=reconstructions.id group by soundchange) as b on a.id=b.soundchange;","top.soundchange_cognate_counts","id","count");
    }
}

void MainWindow::javascript2DArrayFromQuery(QString query, QString varname, QString one, QString two, QString three)
{
    QString thestr;

    thestr += "if(" + varname + " !=null) { delete " + varname + "; }\n";
    thestr += varname + " = Array();\n";

    QList<QString> indices;
    QSqlQuery q(db);
    q.exec(query);

    QSqlRecord rec = q.record();
    int oneCol = rec.indexOf(one);
    int twoCol = rec.indexOf(two);
    int threeCol = rec.indexOf(three);
    while(q.next())
    {
	if(!indices.contains(q.value(oneCol).toString()))
	{
		thestr += varname + "[" + q.value(oneCol).toString()  + "] = Array();\n";
		indices << q.value(oneCol).toString();
	}

	thestr += varname + "[" + q.value(oneCol).toString() + "]["+ q.value(twoCol).toString() +"] = '" + q.value(threeCol).toString().replace("'","\'") + "';\n";
    }

    view->page()->mainFrame()->evaluateJavaScript(thestr);

    writeJavaScriptToFile(thestr);
}

void MainWindow::javascript2DArrayFromQueryArray(QString query, QString varname, QString one, QString two, QString three)
{
    QString thestr;

    thestr += "if(" + varname + " !=null) { delete " + varname + "; }\n";
    thestr += varname + " = Array();\n";

    QList<QString> indices;
    QSqlQuery q(db);
    q.exec(query);

    QSqlRecord rec = q.record();
    int oneCol = rec.indexOf(one);
    int twoCol = rec.indexOf(two);
    int threeCol = rec.indexOf(three);
    while(q.next())
    {
	if(!indices.contains(q.value(oneCol).toString()))
	{
		thestr += varname + "[" + q.value(oneCol).toString()  + "] = Array();\n";
		indices << q.value(oneCol).toString();
	}

	thestr += varname + "[" + q.value(oneCol).toString() + "]["+ q.value(twoCol).toString() +"] = [" + q.value(threeCol).toString().replace("'","\'") + "];\n";
    }

    view->page()->mainFrame()->evaluateJavaScript(thestr);

    writeJavaScriptToFile(thestr);
}


void MainWindow::javascriptFromQuery(QString query, QString varname)
{
    QString thestr;

    thestr += "if(" + varname + " !=null) { delete " + varname + "; }\n";
    thestr += varname + " = Array();\n";

    QSqlQuery q(db);
    q.exec(query);

    QSqlRecord rec = q.record();
    int i, ct=0;
    while(q.next())
    {
    	thestr += varname + "[" + QString::number(ct)  + "] = Array();\n";
	for(i=0; i<rec.count(); i++)
	{
	    if(q.value(i).type()==QVariant::String)
	    {
		    thestr += varname + "["+ QString::number(ct) +"]['" + rec.fieldName(i)  + "'] = '"+ q.value(i).toString().replace("'","\\'") +"';\n";
	    }
	    else
	    {
		thestr += varname + "["+ QString::number(ct) +"]['" + rec.fieldName(i)  + "'] = "+ q.value(i).toString() +";\n";
	    }
	}
	ct++;
    }

    view->page()->mainFrame()->evaluateJavaScript(thestr);

    writeJavaScriptToFile(thestr);
}

void MainWindow::javascript1DArrayFromQuery(QString query, QString varname, QString one, QString value)
{
    QString thestr;

    thestr += "if(" + varname + " !=null) { delete " + varname + "; }\n";
    thestr += varname + " = Array();\n";

    QList<QString> indices;
    QSqlQuery q(db);
    q.exec(query);

    QSqlRecord rec = q.record();
    int oneCol = rec.indexOf(one);
    int valueCol = rec.indexOf(value);
    while(q.next())
    {
	if(!indices.contains(q.value(oneCol).toString()))
	{
		thestr += varname + "[" + q.value(oneCol).toString()  + "] = Array();\n";
		indices << q.value(oneCol).toString();
	}

	thestr += varname + "[" + q.value(oneCol).toString() + "] = '" + q.value(valueCol).toString().replace("'","\'") + "';\n";
    }

    view->page()->mainFrame()->evaluateJavaScript(thestr);
    writeJavaScriptToFile(thestr);
}

void MainWindow::writeJavaScriptToFile(QString thestr)
{
    QFile f("html/context.js");
    if (!f.open(QFile::Append | QFile::Text))
    {
	QMessageBox::warning(this, tr("Codecs"),tr("Can't open the javascipt output file."));
	return;
    }
    QTextStream jsout(&f);
    jsout.setCodec("UTF-8");
    jsout << thestr;
}


bool MainWindow::setupDatabase()
{

    if(!QSqlDatabase::isDriverAvailable("QSQLITE"))
    {
	QMessageBox::critical (0,"Fatal error", "The driver for the database is not available. It is unlikely that you will solve this on your own. Rather you had better contact the developer.");
	return false;
    }
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("database.db");
    if(!db.open())
    {
	    QMessageBox::information (this,"Error Message","There was a problem in opening the database. The program said: " + db.lastError().databaseText() + " It is unlikely that you will solve this on your own. Rather you had better contact the developer." );
	    return false;
    }

/*
    // the answer is no
    if(db.driver()->hasFeature(QSqlDriver::QuerySize))
    {
	QMessageBox::information (this,"Error Message","QuerySize supported.");
    }
    // the answer is yes
    if(db.driver()->hasFeature(QSqlDriver::LastInsertId))
    {
	QMessageBox::information (this,"Error Message","LastInsertId supported.");
    }
    // the answer is yes
    if(db.driver()->hasFeature(QSqlDriver::Transactions))
    {
	QMessageBox::information (this,"Error Message","Transactions supported.");
    }
*/


    QList<QString> tables;
    QSqlQuery q(db);
    q.exec("SELECT name FROM sqlite_master WHERE type='table';");
    while(q.next())
    {
	tables << q.value(0).toString();
    }

    if(!tables.contains("projects"))
	q.exec("create table projects ( name text default NULL, description text default NULL, id integer primary key );");
    if(!tables.contains("approaches"))
	q.exec("create table approaches ( name text default NULL, description text default NULL, project int default NULL, id integer primary key );");
    if(!tables.contains("lemmas"))
	q.exec("create table lemmas ( gloss text default NULL, project int default NULL, id integer primary key );");
    if(!tables.contains("languages"))
	q.exec("create table languages ( fullname text default NULL, abbr text default NULL, iso text default NULL, reconstructed tinyint default 0, philological tinyint default 0, philological_source int default NULL, parent int default NULL, project int default NULL, approach int default NULL, color text default null, id integer primary key );");
    if(!tables.contains("protoforms"))
	q.exec("create table protoforms ( form text default NULL, setid int default NULL, langid int default NULL, id integer primary key );");
    if(!tables.contains("items"))
	q.exec("create table items ( form text default NULL, lemmaid int default NULL, langid int default NULL, project int default NULL, id integer primary key );");
    if(!tables.contains("cognate_sets"))
	q.exec("create table cognate_sets ( name text default NULL, note text default NULL, approach int default NULL, id integer primary key );");
    if(!tables.contains("correspondence_sets"))
	q.exec("create table correspondence_sets ( approach int default NULL, note text default NULL, id integer primary key  );");
    if(!tables.contains("correspondence_set_elements"))
	q.exec("create table correspondence_set_elements ( setid int default NULL, string text default NULL, langid int default NULL );");
    if(!tables.contains("reconstructions"))
	q.exec("create table reconstructions ( string text default NULL, langid int default NULL, note text default NULL, approach int default NULL, id integer primary key );");
    if(!tables.contains("cognate_set_elements"))
	q.exec("create table cognate_set_elements ( setid int default NULL, itemid int default NULL, primary key (setid,itemid) );");
    if(!tables.contains("cognate_correspondence"))
	q.exec("create table cognate_correspondence ( cognate int default NULL, correspondence int default NULL, primary key (cognate, correspondence) );");
    if(!tables.contains("correspondence_reconstruction"))
	q.exec("create table correspondence_reconstruction ( reconstruction int default NULL, correspondence int default NULL, primary key (reconstruction, correspondence) );");
    if(!tables.contains("cookies"))
	q.exec("create table cookies ( name text, value text );");
    if(!tables.contains("soundchanges"))
	q.exec("create table soundchanges ( reconstruction int default NULL, becomes text default NULL, before text default NULL, after text default NULL, inlanguage int default NULL, note text default NULL, follows int default NULL, id integer primary key );");
    if(!tables.contains("soundchange_cognate"))
	q.exec("create table soundchange_cognate ( soundchange int default NULL, cognate int default NULL );");

    return true;

}

void MainWindow::saveCognateNote(const QString &note, int id)
{
    QString n(note);
    QSqlQuery q(db);
    n.replace("'","''");
    q.exec("update cognate_sets set note='"+n+"' where id='"+QString::number(id)+"';");
    RefreshFromDatabase();
}

void MainWindow::saveCorrespondenceNote(const QString &note, int id)
{
    QString n(note);
    QSqlQuery q(db);
    n.replace("'","''");
    q.exec("update correspondence_sets set note='"+n+"' where id='"+QString::number(id)+"';");
    RefreshFromDatabase();
}


void MainWindow::saveReconstructionNote(const QString &note, int id)
{
    QString n(note);
    QSqlQuery q(db);
    n.replace("'","''");
    q.exec("update reconstructions set note='"+n+"' where id='"+QString::number(id)+"';");
    RefreshFromDatabase();
}

void MainWindow::saveSoundchangesNote(const QString &note, int id)
{
    QString n(note);
    QSqlQuery q(db);
    n.replace("'","''");
    q.exec("update soundchanges set note='"+n+"' where id='"+QString::number(id)+"';");
    RefreshFromDatabase();
}


void MainWindow::addProject(const QString &name, const QString &description)
{
    QString n(name),d(description);

    n.replace("'","''");
    d.replace("'","''");

    QSqlQuery q(db);
    q.exec("insert into projects (name,description) values ('" + n  + "','" + d + "');");
    if(q.numRowsAffected()<1)
    {
	QMessageBox::critical(this,"Error", "There was an error inserting '" + n + "' into the database.");
    }
    refreshProjectApproach();
}

void MainWindow::addApproach(const QString &name, const QString &description, int proj)
{
    QString n(name),d(description);

    n.replace("'","''");
    d.replace("'","''");

    QSqlQuery q(db);
    q.exec("insert into approaches (name,description,project) values ('" + n  + "','" + d + "',"+QString::number(proj)+");");
    if(q.numRowsAffected()<1)
    {
	QMessageBox::critical(this,"Error", "There was an error inserting '" + n + "' into the database.");
    }
    refreshProjectApproach();
}

void MainWindow::removeProject(int id)
{
    QSqlQuery q(db);
    q.exec("delete from items where project='"+QString::number(id)+"';");
    q.exec("delete from lemmas where project='"+QString::number(id)+"';");
    q.exec("delete from languages where project='"+QString::number(id)+"';");
    q.exec("delete from projects where id='"+QString::number(id)+"';");
    if(q.numRowsAffected()<1)
    {
	QMessageBox::critical(this,"Error", "There was an error deleting this project from the database.");
    }

    q.exec("select id from approaches where project='"+QString::number(id)+"';");
    while(q.next())
    {
	removeApproach(q.value(0).toInt(),false);
    }

    refreshProjectApproach();
}

void MainWindow::removeApproach(int id, bool update)
{
    QSqlQuery q(db);
    q.exec("delete from protoforms where langid in (select id from languages where approach='"+QString::number(id)+"');");
    q.exec("delete from languages where approach='"+QString::number(id)+"';");
    q.exec("delete from cognate_correspondence where correspondence in (select id from correspondence_sets where approach='"+QString::number(id)+"');");
    q.exec("delete from correspondence_reconstruction where correspondence in (select id from correspondence_sets where approach='"+QString::number(id)+"');");
    q.exec("delete from correspondence_set_elements where setid in (select id from correspondence_sets where approach='"+QString::number(id)+"');");
    q.exec("delete from correspondence_sets where approach='"+QString::number(id)+"';");
    q.exec("delete from cognate_set_elements where setid in (select id from cognate_sets where approach='"+QString::number(id)+"');");
    q.exec("delete from cognate_sets where approach='"+QString::number(id)+"';");
    q.exec("delete from soundchange_cognate where soundchange in (select id from soundchanges where reconstruction in (select id from reconstructions where approach='"+QString::number(id)+"'));");
    q.exec("delete from soundchanges where reconstruction in (select id from reconstructions where approach='"+QString::number(id)+"');");
    q.exec("delete from reconstructions where approach='"+QString::number(id)+"';");
    q.exec("delete from approaches where id='"+QString::number(id)+"';");
    if(q.numRowsAffected()<1)
    {
	QMessageBox::critical(this,"Error", "There was an error deleting this project from the database.");
    }
    if(update)
	refreshProjectApproach();
}

void MainWindow::refreshProjectApproach()
{
    javascriptFromQuery("select id,name,description from projects order by id;","top.projects");
    javascriptFromQuery("select id,name,description,project from approaches order by id;","top.approaches");
}

void MainWindow::receiveFile(const QString &filename)
{
    QString line;
    QStringList list;
    QSqlQuery q;
    QList<int> languageIDs;
    int putativeNumberOfLanguages;
    int lemma;

    db.transaction();

    QFile data(filename);
    if (data.open(QFile::ReadOnly)) {
	QTextStream in(&data);
	in.setCodec("UTF-8");

	// read the first line
	list = parseCSV(in.readLine(10000));
	if(list[0].toLower()!="gloss")
	{
	    QMessageBox::critical(this,"File Format Error", "The first entry of the first row should contain the word 'gloss' (no quotes).");
	    db.rollback();
	    return;
	}
	putativeNumberOfLanguages = list.size()-1;
	for(int i=1; i<list.size(); i++) // remaining languages
	{
	    // important only to escape once
	    list[i].replace("'","''");
	    q.exec("select id,count(id) from languages where project='"+QString::number(project)+"' and fullname='"+list[i]+"';");
	    q.next();
	    if(0==q.value(1).toInt()) // if this language is not in the database, add it
	    {
		q.exec("insert into languages (fullname,project) values ('"+list[i]+"','"+QString::number(project)+"');");
		languageIDs << q.lastInsertId().toInt();
	    }
	    else // remember the order
	    {
		languageIDs << q.value(0).toInt();
	    }
	}

	// remaining lines
	while(!in.atEnd())
	{
	    list = parseCSV(in.readLine(10000));
	    if(list.size()-1!=putativeNumberOfLanguages)
	    {
		QMessageBox::critical(this,"File Format Error", "The expected number of columns was not found near: "+list.first());
		db.rollback();
		return;
	    }

	    // check for the existence of the lemma, possibly add it, store its id in 'lemma'
	    list[0].replace("'","''");
	    q.exec("select id,count(id) from lemmas where project='"+QString::number(project)+"' and gloss='"+list[0]+"';");
	    q.next();
	    if(0==q.value(1).toInt()) // if this lemma is not in the database, add it
	    {
		q.exec("insert into lemmas (gloss,project) values ('"+list[0]+"','"+QString::number(project)+"');");
		lemma = q.lastInsertId().toInt();
	    }
	    else // remember the order
	    {
		lemma = q.value(0).toInt();
	    }

	    for(int i=1; i<list.size(); i++) // remaining forms
	    {
		if(list[i].length()>0)
		{
			q.exec("insert into items (form,langid,lemmaid,project) values ('"+list[i]+"','"+QString::number(languageIDs[i-1])+"','"+QString::number(lemma)+"','"+QString::number(project)+"');");
		}
	    }
	}
    }
    else
    {
	QMessageBox::critical(this,"Error", "This file could not be opened: "+filename);
	db.rollback();
	return;
    }

    db.commit();

    RefreshFromDatabase();
}

QStringList MainWindow::parseCSV(QString line)
{
    line.replace(QRegExp("^\""),"");
    line.replace(QRegExp("\"$"),"");
    QString old="9999999999999999999999999999999999999999999999999";
    while(old!=line)
    {
	old = line;
	line.replace(",,",",\"\",");
    }
    line.replace("\",\"","\t");
    line.replace(QRegExp("^,\""),"\t");
    line.replace(QRegExp("\",$"),"\t");

    QStringList list;
    list = line.split(QRegExp("\t"));

    return list;
}

void MainWindow::modifyData(const QString &form, int lemma, int language)
{
    QString f(form);
    f.replace("'","''");

    QSqlQuery q(db);
    q.exec("update items set form='"+f+"' where lemmaid='"+QString::number(lemma)+"' and langid='"+QString::number(language)+"';");
    refreshItems();
}

void MainWindow::refreshItems()
{
    javascript2DArrayFromQuery("select form,lemmaid,langid from items where project='"+QString::number(project)+"';","top.items","lemmaid","langid","form");
}

void MainWindow::refreshLanguages()
{
    javascriptFromQuery("select id,fullname,reconstructed,abbr,iso,parent,philological,philological_source,color from languages where project='"+QString::number(project)+"' and (reconstructed!='1' or approach='"+QString::number(approach)+"') order by id;","top.languages");
    javascriptFromQuery("select reconstructions.id,langid,fullname,string,color from reconstructions,languages where reconstructions.approach='"+QString::number(approach)+"' and reconstructions.langid=languages.id order by langid;","top.reconstructions");
}

void MainWindow::removeLanguage(int id)
{
    QSqlQuery q;
    q.exec("delete from cognate_set_elements where setid in (select id from items where langid='"+QString::number(id)+"');");
    q.exec("delete from correspondence_set_elements where langid='"+QString::number(id)+"';");
    q.exec("delete from items where langid='"+QString::number(id)+"';");
    q.exec("delete from protoforms where langid='"+QString::number(id)+"';");

    q.exec("delete from soundchange_cognate where soundchange in (select id from soundchanges where reconstruction in (select id from reconstructions where langid='"+QString::number(id)+"'));");
    q.exec("delete from soundchanges where reconstruction in (select id from reconstructions where langid='"+QString::number(id)+"');");
    q.exec("delete from reconstructions where langid='"+QString::number(id)+"';");

    q.exec("delete from languages where id='"+QString::number(id)+"';");
    if(q.numRowsAffected()<1)
    {
	QMessageBox::critical(this,"Error", "There was an error deleting this language from the database.");
    }
    refreshLanguages();
}

void MainWindow::modifyLanguage(int id, const QString &fn, const QString &ab, const QString &iso, int p, int ph, int phs, const QString &col)
{
    QString full(fn);
    full.replace("'","''");
    QString abbr(ab);
    abbr.replace("'","''");
    QString iso_code(iso);
    iso_code.replace("'","''");
    QString color(col);

    QSqlQuery q(db);
    q.exec("update languages set fullname='"+full+"',abbr='"+abbr+"',iso='"+iso_code+"',parent='"+QString::number(p)+"',philological='"+QString::number(ph)+"',philological_source='"+QString::number(phs)+"',color='"+color+"' where id='"+QString::number(id)+"';");
    if(q.numRowsAffected()<1)
    {
	QMessageBox::critical(this,"Error", "There was an error updating this language: " + q.lastError().text());
    }
    refreshLanguages();
}

void MainWindow::createCognateSet(const QString &l, const QString &d, const QString &s, const QString &i)
{
    QString label(l);
    label.replace("'","''");
    QString desc(d);
    desc.replace("'","''");
    QString sel(s);
    QString id(i);
    QSqlQuery q;
    int cogid;

    QStringList items = sel.split(",");
    QStringList languages = id.split(",");

    q.exec("insert into cognate_sets (name,note,approach) values ('" + label + "','" + desc + "','" + QString::number(approach) + "');");
    cogid = q.lastInsertId().toInt();


    for(int i=0; i<items.size(); i++)
    {
	q.exec("insert into cognate_set_elements (setid,itemid) values ('" + QString::number(cogid) + "',(select id from items where lemmaid='" + items.at(i) + "' and langid='" + languages.at(i) + "'));");
    }

    RefreshFromDatabase();
}

void MainWindow::removeCognateSet(int id)
{
    QSqlQuery q(db);

    q.exec("delete from cognate_set_elements where setid='"+QString::number(id)+"';");
    q.exec("delete from cognate_correspondence where cognate='"+QString::number(id)+"';");
    q.exec("delete from soundchange_cognate where cognate='"+QString::number(id)+"';");
    q.exec("delete from protoforms where setid='"+QString::number(id)+"';");
    q.exec("delete from cognate_sets where id='"+QString::number(id)+"';");
    if(q.numRowsAffected()<1)
    {
	QMessageBox::critical(this,"Error", "There was an error deleting this cognate set from the database.");
    }
    RefreshFromDatabase();
}

void MainWindow::alterCognateSet(int id, const QString &s, const QString &i)
{
    QString sel(s);
    QString ids(i);
    QStringList items = sel.split(",");
    QStringList languages = ids.split(",");

    QSqlQuery q(db);
    q.exec("delete from cognate_set_elements where setid='"+QString::number(id)+"';");

    for(int i=0; i<items.size(); i++)
    {
	q.exec("insert into cognate_set_elements (setid,itemid) values ('" + QString::number(id) + "',(select id from items where lemmaid='" + items.at(i) + "' and langid='" + languages.at(i) + "'));");
    }
    RefreshFromDatabase();
}

void MainWindow::reconstructProtoform(int cogid, const QString &f, const QString &l)
{
    QString frms(f);
    QString lgs(l);
    QStringList forms = frms.split(",");
    QStringList languages = lgs.split(",");
    QSqlQuery q(db);

    for(int i=0; i<forms.size(); i++)
    {
	q.exec("delete from protoforms where langid='"+languages[i]+"' and setid='"+QString::number(cogid)+"';");
	q.exec("insert into protoforms (form,langid,setid) values ('"+forms[i]+"','"+languages[i]+"','"+QString::number(cogid)+"');");
    }
    RefreshFromDatabase();
}

void MainWindow::addReconstructedLanguage(const QString &fn, const QString &ab)
{
    QString full(fn);
    full.replace("'","''");
    QString abbr(ab);
    abbr.replace("'","''");

    QSqlQuery q;
    q.exec("insert into languages (fullname,abbr,reconstructed,project,approach) values ('"+full+"','"+abbr+"','1','"+QString::number(project)+"','"+QString::number(approach)+"');");
    if(q.numRowsAffected()<1)
    {
	QMessageBox::critical(this,"Error", "There was an error inserting "+fn+" into the database.");
    }
}

void MainWindow::setProjectApproach(int p, int a)
{
    project = p;
    approach = a;
    RefreshFromDatabase();
}

void MainWindow::createCorrespondenceSet(const QString &s, const QString &i, int cogid)
{
    QString sel(s);
    QString id(i);
    QSqlQuery q;
    int corrid;

    QStringList items = sel.split(","); // items here is really correspondence strings
    QStringList languages = id.split(",");

    q.exec("insert into correspondence_sets (approach) values ('"+QString::number(approach)+"');");
    corrid = q.lastInsertId().toInt();

    for(int i=0; i<items.size(); i++)
    {
	if(items[i]!="-1")
	{
		q.exec("insert into correspondence_set_elements (setid,langid,string) values ('"+QString::number(corrid)+"','"+languages[i]+"','"+items[i]+"');");
	}
    }

    q.exec("insert into cognate_correspondence (cognate,correspondence) values ('"+QString::number(cogid)+"','"+QString::number(corrid)+"');");

    RefreshFromDatabase();
}

void MainWindow::removeCorrespondenceSet(int id)
{
    QSqlQuery q(db);

    q.exec("delete from correspondence_set_elements where setid='"+QString::number(id)+"';");
    q.exec("delete from cognate_correspondence where correspondence='"+QString::number(id)+"';");
    q.exec("delete from correspondence_reconstruction where correspondence='"+QString::number(id)+"';");
    q.exec("delete from correspondence_sets where id='"+QString::number(id)+"';");
    if(q.numRowsAffected()<1)
    {
	QMessageBox::critical(this,"Error", "There was an error deleting this corresponence set from the database.");
    }
    RefreshFromDatabase();
}

void MainWindow::alterCorrespondenceSet(int id, const QString &s, const QString &i)
{
    QString sel(s);
    QString ids(i);
    QStringList items = sel.split(","); // here, really correspondence segments
    QStringList languages = ids.split(",");
    QSqlQuery q(db);

    for(int i=0; i<items.size(); i++)
    {
	if(items[i]!="-1")
	{
	    q.exec("select count(*) from correspondence_set_elements where setid='"+QString::number(id)+"' and langid='"+languages[i]+"';");
	    q.next();
	    if(0==q.value(0).toInt()) // if the correspondence_set_element is not in the database
	    {
		q.exec("insert into correspondence_set_elements (string,langid,setid) values ('"+items[i].replace("'","''")+"','"+languages[i]+"','"+QString::number(id)+"');");
	    }
	    else
	    {
		q.exec("update correspondence_set_elements set string='"+items[i].replace("'","''")+"' where langid='"+languages[i]+"' and setid='"+QString::number(id)+"';");
	    }
	}
	else
	{
	    q.exec("delete from correspondence_set_elements where setid='"+QString::number(id)+"' and langid='"+languages[i]+"';");
	}
    }
    RefreshFromDatabase();
}

void MainWindow::associateCognateCorrespondence(int cognate, int correspondence)
{
    QSqlQuery q;
    q.exec("insert into cognate_correspondence (cognate,correspondence) values ('"+QString::number(cognate)+"','"+QString::number(correspondence)+"');");
    RefreshFromDatabase();
}

void MainWindow::disassociateCognateCorrespondence(int cognate, int correspondence)
{
    QSqlQuery q;
    q.exec("delete from cognate_correspondence  where cognate='"+QString::number(cognate)+"' and correspondence='"+QString::number(correspondence)+"';");
    RefreshFromDatabase();
}

void MainWindow::addReconstruction(const QString &string,int langid,int corrid)
{
    QString s(string);
    QSqlQuery q;
    q.exec("insert into reconstructions (string,langid,approach) values ('"+s+"','"+QString::number(langid)+"','"+QString::number(approach)+"');");
    q.exec("insert into correspondence_reconstruction (correspondence,reconstruction) values ('"+QString::number(corrid)+"','"+QString::number(q.lastInsertId().toInt())+"');");
    RefreshFromDatabase();
}

void MainWindow::removeReconstruction(int id)
{
    QSqlQuery q;
    q.exec("delete from reconstructions where id='"+QString::number(id)+"';");
    q.exec("delete from correspondence_reconstruction where reconstruction='"+QString::number(id)+"';");
    q.exec("delete from soundchange_cognate where soundchange in (select id from soundchanges where reconstruction='"+QString::number(id)+"');");
    q.exec("delete from soundchanges where reconstruction='"+QString::number(id)+"';");
    RefreshFromDatabase();
}

void MainWindow::associateCorrespondenceReconstruction(int correspondence, int reconstruction)
{
    QSqlQuery q;
    q.exec("insert into correspondence_reconstruction (correspondence,reconstruction) values ('"+QString::number(correspondence)+"','"+QString::number(reconstruction)+"');");
    RefreshFromDatabase();
}

void MainWindow::disassociateCorrespondenceReconstruction(int correspondence, int reconstruction)
{
    QSqlQuery q;
    q.exec("delete from correspondence_reconstruction where correspondence='"+QString::number(correspondence)+"' and reconstruction='"+QString::number(reconstruction)+"';");
    RefreshFromDatabase();
}

void MainWindow::createCognateSetsFromRows()
{
    QSqlQuery q,q2;
    q.exec("select distinct lemmaid,gloss from items,lemmas where items.project='"+QString::number(project)+"' and items.lemmaid=lemmas.id;");
    QString gloss;

    while(q.next())
    {
	    q2.exec("insert into cognate_sets (name,approach) values ('"+q.value(1).toString().replace("'","''")+"','"+QString::number(approach)+"');");
	    q2.exec("insert into cognate_set_elements (setid,itemid) select "+q.lastInsertId().toString()+",id from items where lemmaid='"+q.value(0).toString()+"' and project='"+QString::number(project)+"';");
    }
    RefreshFromDatabase();
}

void MainWindow::editReconstruction(int id, const QString &string)
{
    QSqlQuery q;
    QString str(string);
    q.exec("update reconstructions set string='"+str+"' where id='"+QString::number(id)+"';");
    RefreshFromDatabase();
}

void MainWindow::addApproachFromExisting(const QString &name, const QString &description, int project, int approach)
{
    QString nm(name);
    QString dsc(description);
    QSqlQuery q, q2;
    int newapproach, tmpid;

    nm.replace("'","''");
    dsc.replace("'","''");

    db.transaction();

    if(!q.exec("insert into approaches (name,description,project) values ('"+nm+"','"+dsc+"','"+QString::number(project)+"');"))
    {
	QMessageBox::critical(this,"Error", "There was an error and the approach was not copied.");
	db.rollback();
	return;
    }
    newapproach = q.lastInsertId().toInt();

    // create temporary tables
    if(!q.exec("create table tmp_cog ( old int, new int );"))
    {
	QMessageBox::critical(this,"Error", "There was an error and the approach was not copied.");
	db.rollback();
	return;
    }
    if(!q.exec("create table tmp_corr ( old int, new int );"))
    {
	QMessageBox::critical(this,"Error", "There was an error and the approach was not copied.");
	db.rollback();
	return;
    }
    if(!q.exec("create table tmp_recon ( old int, new int );"))
    {
	QMessageBox::critical(this,"Error", "There was an error and the approach was not copied.");
	db.rollback();
	return;
    }
    if(!q.exec("create table tmp_lang ( old int, new int );"))
    {
	QMessageBox::critical(this,"Error", "There was an error and the approach was not copied.");
	db.rollback();
	return;
    }

    // copy cognate sets
    if(!q.exec("select id,name,note from cognate_sets where approach='"+QString::number(approach)+"';"))
    {
	QMessageBox::critical(this,"Error", "There was an error and the approach was not copied. (Cognates)");
	db.rollback();
	return;
    }
    while(q.next())
    {
	if(!q2.exec("insert into cognate_sets (name,note,approach) values ('"+q.value(1).toString().replace("'","''")+"','"+q.value(2).toString().replace("'","''")+"','"+QString::number(newapproach)+"');"))
	{
	    QMessageBox::critical(this,"Error", "There was an error and the approach was not copied. (Cognates)");
	    db.rollback();
	    return;
	}
	tmpid =  q2.lastInsertId().toInt();

	if(!q2.exec("insert into tmp_cog (old,new) values ("+q.value(0).toString()+","+QString::number(tmpid)+");"))
	{
	    QMessageBox::critical(this,"Error", "There was an error and the approach was not copied. (Cognates)");
	    db.rollback();
	    return;
	}

	if(!q2.exec("insert into cognate_set_elements (setid,itemid) select "+QString::number(tmpid)+",itemid from cognate_set_elements where setid='"+q.value(0).toString()+"';"))
	{
	    QMessageBox::critical(this,"Error", "There was an error and the approach was not copied. (Cognates)");
	    db.rollback();
	    return;
	}
    }

    // copy correspondences
    if(!q.exec("select id,note from correspondence_sets where approach='"+QString::number(approach)+"';"))
    {
	QMessageBox::critical(this,"Error", "There was an error and the approach was not copied. (Correspondences)");
	db.rollback();
	return;
    }
    while(q.next())
    {
	if(!q2.exec("insert into correspondence_sets (approach,note) values ('"+QString::number(newapproach)+"','"+q.value(1).toString()+"');"))
	{
	    QMessageBox::critical(this,"Error", "There was an error and the approach was not copied. (Correspondences)");
	    db.rollback();
	    return;
	}
	tmpid =  q2.lastInsertId().toInt();

	if(!q2.exec("insert into tmp_corr (old,new) values ("+q.value(0).toString()+","+QString::number(tmpid)+");"))
	{
	    QMessageBox::critical(this,"Error", "There was an error and the approach was not copied. (Correspondences)");
	    db.rollback();
	    return;
	}
	if(!q2.exec("insert into correspondence_set_elements (setid,string,langid) select "+QString::number(tmpid)+",string,langid from correspondence_set_elements where setid='"+q.value(0).toString()+"';"))
	{
	    QMessageBox::critical(this,"Error", "There was an error and the approach was not copied. (Correspondences)");
	    db.rollback();
	    return;
	}
    }

    // copy reconstructions
    if(!q.exec("select id, string, langid, note, approach from reconstructions where approach='"+QString::number(approach)+"';"))
    {
	QMessageBox::critical(this,"Error", "There was an error and the approach was not copied. (Reconstructions)");
	db.rollback();
	return;
    }
    while(q.next())
    {
	if(!q2.exec("insert into reconstructions (string,langid,note,approach) values ('"+q.value(1).toString()+"','"+q.value(2).toString()+"','"+q.value(3).toString()+"','"+QString::number(newapproach)+"');"))
	{
	    QMessageBox::critical(this,"Error", "There was an error and the approach was not copied. (Reconstructions)");
	    db.rollback();
	    return;
	}
	tmpid =  q2.lastInsertId().toInt();

	if(!q2.exec("insert into tmp_recon (old,new) values ("+q.value(0).toString()+","+QString::number(tmpid)+");"))
	{
	    QMessageBox::critical(this,"Error", "There was an error and the approach was not copied. (Reconstructions)");
	    db.rollback();
	    return;
	}
    }

    // copy languages
    if(!q.exec("select id,fullname,abbr,iso,reconstructed,project,approach from languages where approach='"+QString::number(approach)+"';"))
    {
	QMessageBox::critical(this,"Error", "There was an error and the approach was not copied. (Languages)");
	db.rollback();
	return;
    }
    while(q.next())
    {
	if(!q2.exec("insert into languages (fullname,abbr,iso,reconstructed,project,approach) values ('"+q.value(1).toString()+"','"+q.value(2).toString()+"','"+q.value(3).toString()+"','"+q.value(4).toString()+"','"+q.value(5).toString()+"','"+QString::number(newapproach)+"');"))
	{
	    QMessageBox::critical(this,"Error", "There was an error and the approach was not copied. (Languages)");
	    db.rollback();
	    return;
	}
	tmpid =  q2.lastInsertId().toInt();

	if(!q2.exec("insert into tmp_lang (old,new) values ("+q.value(0).toString()+","+QString::number(tmpid)+");"))
	{
	    QMessageBox::critical(this,"Error", "There was an error and the approach was not copied. (Languages)");
	    db.rollback();
	    return;
	}
    }

    // copy protoforms
    if(!q.exec("select protoforms.id,form,setid,langid from protoforms,languages where languages.approach='"+QString::number(approach)+"' and languages.id=protoforms.langid;"))
    {
	QMessageBox::critical(this,"Error", "There was an error and the approach was not copied. (Protoforms)");
	db.rollback();
	return;
    }
    while(q.next())
    {
	if(!q2.exec("insert into protoforms (form,setid,langid) select form,setid,tmp_lang.new as langid from protoforms,tmp_lang,languages where tmp_lang.old=protoforms.langid;"))
	{
	    QMessageBox::critical(this,"Error", "There was an error and the approach was not copied. (Protoforms)");
	    db.rollback();
	    return;
	}
    }

    // cognate/correspondence table
    if(!q.exec("insert into cognate_correspondence (cognate,correspondence) select tmp_cog.new as cognate,tmp_corr.new as correspondence from cognate_correspondence,tmp_cog,tmp_corr where cognate_correspondence.cognate=tmp_cog.old and cognate_correspondence.correspondence=tmp_corr.old;"))
    {
	QMessageBox::critical(this,"Error", "There was an error and the approach was not copied.");
	db.rollback();
	return;
    }

    // correspondence/reconstruction table
    if(!q.exec("insert into correspondence_reconstruction (correspondence,reconstruction) select tmp_corr.new as correspondence,tmp_recon.new as reconstruction from correspondence_reconstruction,tmp_recon,tmp_corr where correspondence_reconstruction.reconstruction=tmp_recon.old and correspondence_reconstruction.correspondence=tmp_corr.old;"))
    {
	QMessageBox::critical(this,"Error", "There was an error and the approach was not copied.");
	db.rollback();
	return;
    }

    // remove temporary tables
    if(!q.exec("drop table tmp_cog;"))
    {
	QMessageBox::critical(this,"Error", "There was an error and the approach was not copied.");
	db.rollback();
	return;
    }
    if(!q.exec("drop table tmp_corr;"))
    {
	QMessageBox::critical(this,"Error", "There was an error and the approach was not copied.");
	db.rollback();
	return;
    }
    if(!q.exec("drop table tmp_recon;"))
    {
	QMessageBox::critical(this,"Error", "There was an error and the approach was not copied.");
	db.rollback();
	return;
    }
    if(!q.exec("drop table tmp_lang;"))
    {
	QMessageBox::critical(this,"Error", "There was an error and the approach was not copied.");
	db.rollback();
	return;
    }

    db.commit();
    RefreshFromDatabase();
}

void MainWindow::exportXML(bool strings, bool indices, bool data, bool cognate, bool correspondence, bool reconstructions)
{
    QString theXML;
    QXmlStreamWriter xmlstream(&theXML);
    QSqlQuery q, q2, q3;
    xmlstream.setCodec("UTF-8");
    xmlstream.setAutoFormatting(true);
    xmlstream.writeStartDocument();
    xmlstream.writeStartElement("root");

    xmlstream.writeStartElement("information");
	xmlstream.writeStartElement("project");
	    q.exec("select name,description from projects where id='"+QString::number(project)+"';");
	    q.next();
	    xmlstream.writeTextElement("name",q.value(0).toString());
	    xmlstream.writeTextElement("description",q.value(1).toString());
	xmlstream.writeEndElement();
	xmlstream.writeStartElement("approach");
	    q.exec("select name,description from approaches where id='"+QString::number(approach)+"';");
	    q.next();
	    xmlstream.writeTextElement("name",q.value(0).toString());
	    xmlstream.writeTextElement("description",q.value(1).toString());
	xmlstream.writeEndElement();
    xmlstream.writeEndElement(); // information

    if(indices)
    {
	xmlstream.writeStartElement("lemmas");
	q.exec("select id,gloss from lemmas where project='"+QString::number(project)+"';");
	while(q.next())
	{
	    xmlstream.writeStartElement("lemma");
	    xmlstream.writeAttribute("id",q.value(0).toString());
	    xmlstream.writeCharacters(q.value(1).toString());
	    xmlstream.writeEndElement();
	}
	xmlstream.writeEndElement(); // lemmas
    }

    xmlstream.writeStartElement("languages");
    q.exec("select fullname, abbr, iso, reconstructed, philological, philological_source, parent, id, color from languages where (project='"+QString::number(project)+"') and (reconstructed='0' or approach='"+QString::number(approach)+"');");
    while(q.next())
    {
	xmlstream.writeStartElement("language");
	if(indices)
	    xmlstream.writeAttribute("id",q.value(7).toString());
	xmlstream.writeAttribute("reconstructed",q.value(4).toString());
	xmlstream.writeAttribute("philological",q.value(4).toString());
	xmlstream.writeTextElement("name",q.value(0).toString());
	xmlstream.writeTextElement("abbreviation",q.value(1).toString());
	xmlstream.writeTextElement("iso639-3",q.value(2).toString());
	xmlstream.writeTextElement("color",q.value(8).toString());
	if(q.value(4).toInt())
	{
	    if(strings && indices)
	    {
		xmlstream.writeStartElement("philological_source");
		xmlstream.writeAttribute("id",q.value(5).toString());
		q2.exec("select fullname from languages where id='"+q.value(5).toString()+"';");
		q2.next();
		xmlstream.writeCharacters(q2.value(0).toString());
		xmlstream.writeEndElement();
	    }
	    else if(strings)
	    {
		q2.exec("select fullname from languages where id='"+q.value(5).toString()+"';");
		q2.next();
		xmlstream.writeTextElement("philological_source",q2.value(0).toString());
	    }
	    else if(indices)
	    {
		xmlstream.writeEmptyElement("philological_source");
		xmlstream.writeAttribute("id",q.value(5).toString());
	    }
	}

	if(q.value(6).toInt()!=0)
	{
	    if(strings && indices)
	    {
		xmlstream.writeStartElement("parent");
		xmlstream.writeAttribute("id",q.value(6).toString());
		q2.exec("select fullname from languages where id='"+q.value(6).toString()+"';");
		q2.next();
		xmlstream.writeCharacters(q2.value(0).toString());
		xmlstream.writeEndElement();
	    }
	    else if(strings)
	    {
		q2.exec("select fullname from languages where id='"+q.value(6).toString()+"';");
		q2.next();
		xmlstream.writeTextElement("parent",q2.value(0).toString());
	    }
	    else if(indices)
	    {
		xmlstream.writeEmptyElement("parent");
		xmlstream.writeAttribute("id",q.value(6).toString());
	    }
	}
	xmlstream.writeEndElement();
    }
    xmlstream.writeEndElement(); // languages


    if(data)
    {
	xmlstream.writeStartElement("data");

	q.exec("select items.id,items.form,items.lemmaid,items.langid,languages.fullname,lemmas.gloss from items,languages,lemmas where items.langid=languages.id and items.lemmaid=lemmas.id and items.project='"+QString::number(project)+"' order by lemmaid,langid;");
	if(indices && strings)
	{
	    while(q.next())
	    {
		xmlstream.writeStartElement("datum");
		xmlstream.writeAttribute("id",q.value(0).toString());
		xmlstream.writeAttribute("lemma",q.value(2).toString());
		xmlstream.writeAttribute("language",q.value(3).toString());
		xmlstream.writeTextElement("form",q.value(1).toString());
		xmlstream.writeTextElement("language",q.value(4).toString());
		xmlstream.writeTextElement("lemma",q.value(5).toString());
		xmlstream.writeEndElement();
	    }
	}
	else if(indices)
	{
	    while(q.next())
	    {
		xmlstream.writeStartElement("datum");
		xmlstream.writeAttribute("id",q.value(0).toString());
		xmlstream.writeAttribute("lemma",q.value(2).toString());
		xmlstream.writeAttribute("language",q.value(3).toString());
		xmlstream.writeCharacters(q.value(1).toString());
		xmlstream.writeEndElement();
	    }
	}
	else if(strings)
	{
	    while(q.next())
	    {
		xmlstream.writeStartElement("datum");
		xmlstream.writeTextElement("form",q.value(1).toString());
		xmlstream.writeTextElement("language",q.value(4).toString());
		xmlstream.writeTextElement("lemma",q.value(5).toString());
		xmlstream.writeEndElement();
	    }
	}

	xmlstream.writeEndElement(); // data
    }
    if(cognate)
    {
	xmlstream.writeStartElement("cognatesets");

	q.exec("select id,name,note from cognate_sets where approach='"+QString::number(approach)+"';");

	if(indices && strings)
	{
	    while(q.next())
	    {
		xmlstream.writeStartElement("cognateset");
		xmlstream.writeTextElement("name",q.value(1).toString());
		xmlstream.writeTextElement("note",q.value(2).toString());
		q2.exec("select cognate_set_elements.itemid,items.form,items.langid,languages.fullname from cognate_set_elements,items,languages where cognate_set_elements.itemid=items.id and items.langid=languages.id and cognate_set_elements.setid='"+q.value(0).toString()+"';");
		while(q2.next())
		{
		    xmlstream.writeStartElement("element");
		    xmlstream.writeAttribute("item",q2.value(0).toString());
		    xmlstream.writeAttribute("language",q2.value(2).toString());
		    xmlstream.writeTextElement("language",q2.value(3).toString());
		    xmlstream.writeTextElement("form",q2.value(1).toString());
		    xmlstream.writeEndElement();
		}
		q2.exec("select protoforms.id,form,langid,fullname from protoforms,languages where setid='"+q.value(0).toString()+"' and protoforms.langid=languages.id;");
		while(q2.next())
		{
		    xmlstream.writeStartElement("protoform");
		    xmlstream.writeAttribute("id",q2.value(0).toString());
		    xmlstream.writeAttribute("language",q2.value(2).toString());
		    xmlstream.writeTextElement("form",q2.value(1).toString());
		    xmlstream.writeTextElement("language",q2.value(3).toString());
		    xmlstream.writeEndElement();
		}
		xmlstream.writeEndElement();
	    }
	}
	else if(indices)
	{
	    while(q.next())
	    {
		xmlstream.writeStartElement("cognateset");
		xmlstream.writeTextElement("name",q.value(1).toString());
		xmlstream.writeTextElement("note",q.value(2).toString());
		q2.exec("select itemid,langid from cognate_set_elements,items where itemid=items.id and setid='"+q.value(0).toString()+"';");
		while(q2.next())
		{
		    xmlstream.writeEmptyElement("element");
		    xmlstream.writeAttribute("item",q2.value(0).toString());
		    xmlstream.writeAttribute("language",q2.value(1).toString());
		}
		q2.exec("select protoforms.id,form,langid from protoforms where setid='"+q.value(0).toString()+"';");
		while(q2.next())
		{
		    xmlstream.writeStartElement("protoform");
		    xmlstream.writeAttribute("id",q2.value(0).toString());
		    xmlstream.writeAttribute("language",q2.value(2).toString());
		    xmlstream.writeTextElement("form",q2.value(1).toString());
		    xmlstream.writeEndElement();
		}
		xmlstream.writeEndElement();
	    }
	}
	else if(strings)
	{
	    while(q.next())
	    {
		xmlstream.writeStartElement("cognateset");
		xmlstream.writeTextElement("name",q.value(1).toString());
		xmlstream.writeTextElement("note",q.value(2).toString());
		q2.exec("select items.form,languages.fullname from cognate_set_elements,items,languages where cognate_set_elements.itemid=items.id and items.langid=languages.id and cognate_set_elements.setid='"+q.value(0).toString()+"';");
		while(q2.next())
		{
		    xmlstream.writeStartElement("element");
		    xmlstream.writeTextElement("language",q2.value(1).toString());
		    xmlstream.writeTextElement("form",q2.value(0).toString());
		    xmlstream.writeEndElement();
		}
		q2.exec("select form,fullname from protoforms,languages where setid='"+q.value(0).toString()+"' and protoforms.langid=languages.id;");
		while(q2.next())
		{
		    xmlstream.writeStartElement("protoform");
		    xmlstream.writeTextElement("form",q2.value(0).toString());
		    xmlstream.writeTextElement("language",q2.value(1).toString());
		    xmlstream.writeEndElement();
		}
		xmlstream.writeEndElement();
	    }
	}
	xmlstream.writeEndElement(); // cognatesets
    }
    if(correspondence)
    {
	xmlstream.writeStartElement("correspondencesets");
	q.exec("select id,note from correspondence_sets where approach='"+QString::number(approach)+"';");
	while(q.next())
	{
	    xmlstream.writeStartElement("correspondenceset");
	    if(indices)
		xmlstream.writeAttribute("id",q.value(0).toString());
	    xmlstream.writeTextElement("note",q.value(1).toString());
	    if(indices && strings)
	    {
		q2.exec("select string,langid,fullname from correspondence_set_elements,languages where languages.id=correspondence_set_elements.langid and setid='"+q.value(0).toString()+"';");
		while(q2.next())
		{
		    xmlstream.writeStartElement("element");
		    xmlstream.writeAttribute("language",q2.value(1).toString());
		    xmlstream.writeTextElement("form",q2.value(0).toString());
		    xmlstream.writeTextElement("language",q2.value(2).toString());

		    q3.exec("select cognate from cognate_correspondence where correspondence='"+q.value(0).toString()+"';");
		    while(q3.next())
		    {
			xmlstream.writeEmptyElement("cognateset");
			xmlstream.writeAttribute("id",q3.value(0).toString());
		    }

		    xmlstream.writeEndElement();
		}
	    }
	    else if(indices)
	    {
		q2.exec("select string,langid from correspondence_set_elements where setid='"+q.value(0).toString()+"';");
		while(q2.next())
		{
		    xmlstream.writeStartElement("element");
		    xmlstream.writeAttribute("language",q2.value(1).toString());
		    xmlstream.writeTextElement("form",q2.value(0).toString());

		    q3.exec("select cognate from cognate_correspondence where correspondence='"+q.value(0).toString()+"';");
		    while(q3.next())
		    {
			xmlstream.writeEmptyElement("cognateset");
			xmlstream.writeAttribute("id",q3.value(0).toString());
		    }

		    xmlstream.writeEndElement();
		}
	    }
	    else if(strings)
	    {
		q2.exec("select string,fullname from correspondence_set_elements,languages where languages.id=correspondence_set_elements.langid and setid='"+q.value(0).toString()+"';");
		while(q2.next())
		{
		    xmlstream.writeStartElement("element");
		    xmlstream.writeTextElement("form",q2.value(0).toString());
		    xmlstream.writeTextElement("language",q2.value(1).toString());
		    xmlstream.writeEndElement();
		}
	    }
	    xmlstream.writeEndElement();
	}
	xmlstream.writeEndElement(); // correspondencesets
    }
    if(reconstructions)
    {
	if(indices && strings)
	{
	    xmlstream.writeStartElement("reconstructions");
	    q.exec("select reconstructions.id,string,langid,note,fullname from reconstructions,languages where reconstructions.approach='"+QString::number(approach)+"' and languages.id=reconstructions.langid;");
	    while(q.next())
	    {
		xmlstream.writeStartElement("reconstruction");
		xmlstream.writeAttribute("id",q.value(0).toString());
		xmlstream.writeAttribute("language",q.value(2).toString());
		xmlstream.writeTextElement("form",q.value(1).toString());
		xmlstream.writeTextElement("language",q.value(4).toString());
		xmlstream.writeTextElement("note",q.value(3).toString());

		q2.exec("select correspondence from correspondence_reconstruction where reconstruction='"+q.value(0).toString()+"';");
		while(q2.next())
		{
		    xmlstream.writeEmptyElement("correspondenceset");
		    xmlstream.writeAttribute("id",q2.value(0).toString());
		}
		xmlstream.writeEndElement();
	    }
	    xmlstream.writeEndElement(); // reconstructions

	    xmlstream.writeStartElement("soundchanges");
	    q.exec("select soundchanges.id,reconstruction,(select fullname from languages,reconstructions where reconstructions.id=reconstruction and reconstructions.langid=languages.id) as srclanguage,(select string from reconstructions where id=reconstruction) as string,becomes,before,after,inlanguage as inlanguageID,(select fullname from languages where id=inlanguage) as inlanguage,soundchanges.note from soundchanges,reconstructions where reconstructions.approach='"+QString::number(approach)+"' and soundchanges.reconstruction=reconstructions.id;");
	    while(q.next())
	    {
		xmlstream.writeStartElement("soundchange");
		xmlstream.writeAttribute("id",q.value(0).toString());
		xmlstream.writeTextElement("source_language",q.value(2).toString());

		xmlstream.writeStartElement("target");
		xmlstream.writeAttribute("reconstruction_id",q.value(1).toString());
		xmlstream.writeCharacters(q.value(3).toString());
		xmlstream.writeEndElement();

		xmlstream.writeTextElement("becomes",q.value(4).toString());
		xmlstream.writeTextElement("before",q.value(5).toString());
		xmlstream.writeTextElement("after",q.value(6).toString());

		xmlstream.writeStartElement("in_language");
		xmlstream.writeAttribute("id",q.value(7).toString());
		xmlstream.writeCharacters(q.value(8).toString());
		xmlstream.writeEndElement();

		xmlstream.writeTextElement("note",q.value(9).toString());

		q2.exec("select (select id from protoforms where cognate=setid) as id,cognate from soundchange_cognate where soundchange='"+q.value(0).toString()+"';");
		while(q2.next())
		{
		    xmlstream.writeStartElement("protoform");
		    if(q2.value(0).toString().length()>0)
			    xmlstream.writeAttribute("protoform",q2.value(0).toString());
		    xmlstream.writeAttribute("cognateset",q2.value(1).toString());
		    xmlstream.writeEndElement();
		}

		xmlstream.writeEndElement();
	    }
	    xmlstream.writeEndElement(); // soundchanges


	}
	else if(indices)
	{
	    xmlstream.writeStartElement("reconstructions");
	    q.exec("select id,string,langid,note from reconstructions where reconstructions.approach='"+QString::number(approach)+"';");
	    while(q.next())
	    {
		xmlstream.writeStartElement("reconstruction");
		xmlstream.writeAttribute("id",q.value(0).toString());
		xmlstream.writeAttribute("language",q.value(2).toString());
		xmlstream.writeTextElement("form",q.value(1).toString());
		xmlstream.writeTextElement("note",q.value(3).toString());

		q2.exec("select correspondence from correspondence_reconstruction where reconstruction='"+q.value(0).toString()+"';");
		while(q2.next())
		{
		    xmlstream.writeEmptyElement("correspondenceset");
		    xmlstream.writeAttribute("id",q2.value(0).toString());
		}

		xmlstream.writeEndElement();
	    }
	    xmlstream.writeEndElement(); // reconstructions

	    xmlstream.writeStartElement("soundchanges");
	    q.exec("select soundchanges.id, reconstruction, becomes, before, after, inlanguage, soundchanges.note from soundchanges,reconstructions where reconstructions.approach='"+QString::number(approach)+"' and soundchanges.reconstruction=reconstructions.id;");
	    while(q.next())
	    {
		xmlstream.writeStartElement("soundchange");
		xmlstream.writeAttribute("id",q.value(0).toString());
		xmlstream.writeTextElement("reconstruction",q.value(1).toString());
		xmlstream.writeTextElement("becomes",q.value(2).toString());
		xmlstream.writeTextElement("before",q.value(3).toString());
		xmlstream.writeTextElement("after",q.value(4).toString());
		xmlstream.writeTextElement("in_language",q.value(5).toString());
		xmlstream.writeTextElement("note",q.value(6).toString());

		q2.exec("select (select id from protoforms where cognate=setid) as id,cognate from soundchange_cognate where soundchange='"+q.value(0).toString()+"';");
		while(q2.next())
		{
		    xmlstream.writeStartElement("protoform");
		    if(q2.value(0).toString().length()>0)
			    xmlstream.writeAttribute("protoform",q2.value(0).toString());
		    xmlstream.writeAttribute("cognateset",q2.value(1).toString());
		    xmlstream.writeEndElement();
		}

		xmlstream.writeEndElement();
	    }
	    xmlstream.writeEndElement(); // soundchanges
	}
	else if(strings)
	{
	    xmlstream.writeStartElement("reconstructions");
	    q.exec("select string,note,fullname from reconstructions,languages where reconstructions.approach='"+QString::number(approach)+"' and languages.id=reconstructions.langid;");
	    while(q.next())
	    {
		xmlstream.writeStartElement("reconstruction");
		xmlstream.writeTextElement("form",q.value(0).toString());
		xmlstream.writeTextElement("language",q.value(2).toString());
		xmlstream.writeTextElement("note",q.value(1).toString());
		xmlstream.writeEndElement();
	    }
	    xmlstream.writeEndElement(); // reconstructions

	    xmlstream.writeStartElement("soundchanges");
	    q.exec("select (select fullname from languages,reconstructions where reconstructions.id=reconstruction and reconstructions.langid=languages.id) as srclanguage,(select string from reconstructions where id=reconstruction) as string,becomes,before,after,(select fullname from languages where id=inlanguage) as inlanguage,soundchanges.note from soundchanges,reconstructions where reconstructions.approach='"+QString::number(approach)+"' and soundchanges.reconstruction=reconstructions.id;");
	    while(q.next())
	    {
		xmlstream.writeStartElement("soundchange");

		xmlstream.writeTextElement("source_language",q.value(0).toString());
		xmlstream.writeTextElement("target",q.value(1).toString());

		xmlstream.writeTextElement("becomes",q.value(2).toString());
		xmlstream.writeTextElement("before",q.value(3).toString());
		xmlstream.writeTextElement("after",q.value(4).toString());
		xmlstream.writeTextElement("in_language",q.value(5).toString());
		xmlstream.writeTextElement("note",q.value(6).toString());

		xmlstream.writeEndElement();
	    }
	    xmlstream.writeEndElement(); // soundchanges
	}

    }

    xmlstream.writeEndDocument(); // root


    QString fileName = QFileDialog::getSaveFileName(this,tr("Save File"), "", tr("XML Files (*.xml)"));
    if (!fileName.isEmpty())
    {
	QFile file(fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text))
	{
	    QMessageBox::warning(this, tr("Codecs"),tr("Cannot write file %1:\n%2").arg(fileName).arg(file.errorString()));
	    return;
	}

	QTextStream out(&file);
	out.setCodec("UTF-8");
	out << theXML;
    }

}


void MainWindow::setCookie(const QString &name, const QString &value)
{
    QSqlQuery q;
    QString n(name);
    QString v(value);
    n.replace("'","''");
    v.replace("'","''");
    q.exec("delete from cookies where name='"+n+"';");
    q.exec("insert into cookies (name,value) values ('"+n+"','"+v+"');");
}

QString MainWindow::getCookies()
{
    QString str="";
    QSqlQuery q;
    q.exec("select name,value from cookies;");

    while(q.next())
    {
	str += q.value(0).toString() + "=" + q.value(1).toString() + "; ";
    }
    str.remove(QRegExp("; $"));

    return str;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    view->page()->mainFrame()->evaluateJavaScript("top.setCookieSettings();");

    QMainWindow::closeEvent(event);
}

// Sound Change Functions


void MainWindow::associateSoundchangeCognate(int soundchange, int cognate)
{
    QSqlQuery q;
    q.exec("insert into soundchange_cognate (soundchange,cognate) values ('"+QString::number(soundchange)+"','"+QString::number(cognate)+"');");
    RefreshFromDatabase();
}

void MainWindow::disassociateSoundchangeCognate(int soundchange, int cognate)
{
    QSqlQuery q;
    q.exec("delete from soundchange_cognate  where soundchange='"+QString::number(soundchange)+"' and cognate='"+QString::number(cognate)+"';");
    RefreshFromDatabase();
}

void MainWindow::createSoundChange(int r, const QString &bec, const QString &b, const QString &a, int inlanguage, int follows, int correspondence)
{
    QString becomes(bec);
    QString before(b);
    QString after(a);
    QSqlQuery q;

    q.exec("insert into soundchanges (reconstruction,becomes,before,after,inlanguage,follows) values ('"+QString::number(r)+"','"+becomes+"','"+before+"','"+after+"','"+QString::number(inlanguage)+"','"+QString::number(follows)+"');");

    RefreshFromDatabase();
}

void MainWindow::removeSoundChange(int id)
{
    QSqlQuery q;
    q.exec("delete from soundchanges where id='"+QString::number(id)+"';");
    q.exec("delete from soundchange_cognate where soundchange='"+QString::number(id)+"';");
    q.exec("update soundchanges set follows='-1' where follows='"+QString::number(id)+"';");
    RefreshFromDatabase();
}

void MainWindow::alterSoundChange(int id, int r, const QString &bec, const QString &b, const QString &a, int inlanguage, int follows)
{
    QString becomes(bec);
    QString before(b);
    QString after(a);
    QSqlQuery q;
    q.exec("update soundchanges set reconstruction='"+QString::number(r)+"',becomes='"+becomes+"',before='"+before+"',after='"+after+"',inlanguage='"+QString::number(inlanguage)+"',inlanguage='"+QString::number(follows)+"' where id='"+QString::number(id)+"';");
    RefreshFromDatabase();
}

