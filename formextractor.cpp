#include "formextractor.h"

FormExtractor::FormExtractor(QWidget *parent, Qt::WFlags flags)
    : QWidget(parent, flags)
{
    ui.setupUi(this);
    ui.webView->setUrl(QUrl("qrc:/form.html"));
    connect(ui.webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(populateJavaScriptWindowObject()));
    resize(300, 300);
}

FormExtractor::~FormExtractor()
{
}

void FormExtractor::setValues(const QString &firstName, const QString &lastName,
                              const QString &gender, bool updates)
{
    ui.firstNameEdit->setText(firstName);
    ui.lastNameEdit->setText(lastName);
    ui.genderEdit->setText(gender);

    if (updates == false)
        ui.updatesEdit->setText("No");
    else
        ui.updatesEdit->setText("Yes");
}

void FormExtractor::populateJavaScriptWindowObject()
{
    ui.webView->page()->mainFrame()->addToJavaScriptWindowObject("formExtractor", this);
}

void FormExtractor::ReceiveMessage()
{
    QMessageBox::information (this,"Success Message","Well hello" );
}
