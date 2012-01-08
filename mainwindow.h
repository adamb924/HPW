#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QSqlDatabase>

QT_BEGIN_NAMESPACE
class QWebView;
class QLineEdit;
class QLineEdit;
class QToolbar;
class QMessageBox;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT


private:
    QString jQuery;
    QWebView *view;
    QLineEdit *locationEdit;
    int progress;

    QSqlDatabase db;

    void javascript2DArrayFromQuery(QString query, QString varname, QString one, QString two, QString three);
    void javascript2DArrayFromQueryArray(QString query, QString varname, QString one, QString two, QString three);
    void javascriptFromQuery(QString query, QString varname);
    void javascript1DArrayFromQuery(QString query, QString varname, QString one, QString value);
    int project, approach;
    void writeJavaScriptToFile(QString thestr);

    QStringList parseCSV(QString line);    

    void closeEvent(QCloseEvent *event);

public slots:
    void RefreshFromDatabase();
    void populateJavaScriptWindowObject();
    bool setupDatabase();
    void addProject(const QString &name, const QString &description);
    void addApproach(const QString &name, const QString &description, int project);
    void addApproachFromExisting(const QString &name, const QString &description, int project, int approach);

    void removeProject(int id);
    void removeApproach(int id, bool update=true);
    void receiveFile(const QString &filename);
    void modifyData(const QString &form, int lemma, int language);
    void removeLanguage(int id);
    void modifyLanguage(int id, const QString &fn, const QString &ab, const QString &iso, int p, int ph, int phs, const QString &col);
    void refreshProjectApproach();
    void refreshItems();
    void refreshLanguages();
    void createCognateSetsFromRows();
    void createCognateSet(const QString &l, const QString &d, const QString &s, const QString &i);
    void removeCognateSet(int id);
    void alterCognateSet(int id, const QString &s, const QString &i);
    void reconstructProtoform(int cogid, const QString &f, const QString &l);
    void addReconstructedLanguage(const QString &fn, const QString &ab);
    void setProjectApproach(int p, int a);
    void saveCognateNote(const QString &note, int id);
    void saveCorrespondenceNote(const QString &note, int id);
    void saveReconstructionNote(const QString &note, int id);
    void saveSoundchangesNote(const QString &note, int id);

    void createCorrespondenceSet(const QString &s, const QString &i, int cogid);
    void removeCorrespondenceSet(int id);
    void alterCorrespondenceSet(int id, const QString &s, const QString &i);
    void associateCognateCorrespondence(int cognate, int correspondence);
    void disassociateCognateCorrespondence(int cognate, int correspondence);

    void addReconstruction(const QString &string,int langid, int corrid);
    void removeReconstruction(int id);
    void associateCorrespondenceReconstruction(int correspondence, int reconstruction);
    void disassociateCorrespondenceReconstruction(int correspondence, int reconstruction);
    void editReconstruction(int id, const QString &string);

    void createSoundChange(int r, const QString &bec, const QString &b, const QString &a, int inlanguage, int follows, int correspondence);
    void removeSoundChange(int id);
    void alterSoundChange(int id, int r, const QString &bec, const QString &b, const QString &a, int inlanguage, int follows);
    void associateSoundchangeCognate(int soundchange, int cognate);
    void disassociateSoundchangeCognate(int soundchange, int cognate);

    void setCookie(const QString &name, const QString &value);
    QString getCookies();

    void exportXML(bool strings, bool indices, bool data, bool cognate, bool correspondence, bool reconstructions);

public:
    bool unrecoverableError;
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
};

#endif // MAINWINDOW_H
