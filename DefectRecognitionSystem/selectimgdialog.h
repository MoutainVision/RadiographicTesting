#ifndef SELECTIMGDIALOG_H
#define SELECTIMGDIALOG_H

#include <QDialog>
#include <QDateTime>
#include <QMessageBox>

#include "appconfig.h"
#include "readdcmfile.h"

#include "prewdg.h"

#include "ImageRetrieval.h"


namespace Ui {
class SelectImgDialog;
}

class SelectImgDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectImgDialog(QWidget *parent = nullptr);
    ~SelectImgDialog();

    void setSimilarValues(vector<RetrievalResult> aRes);

    void setCurrentInfo(DcmFileNode curInfo);

    void setDCMFileInfo(DcmFileNode srcInfo, DcmFileNode similarInfo);

    DcmFileNode getFileInfo();

private slots:
    void slot_tableCellClicked(int row, int col);

private:
    Ui::SelectImgDialog *ui;

    PreWdg *mSrcWdg;
    PreWdg *mSimilarWdg;

    int     mCurIndex;

    DcmFileNode mCurFileInfo;
    DcmFileNode mDbFileInfo;

};

#endif // SELECTIMGDIALOG_H
