#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->statusBar->showMessage(tr("Ready"));

    mpGraphicAreaWidget = new GraphicAreaWidget(this);
    ui->horizontalLayoutPaint->addWidget(mpGraphicAreaWidget);
    ui->horizontalLayoutPaint->setStretch(0,0);
    ui->horizontalLayoutPaint->setStretch(1,100);
}

MainWindow::~MainWindow() {
    delete ui;
    delete mpGraphicAreaWidget;
}

void MainWindow::on_actionExit_triggered() {
    close();
}

void MainWindow::on_actionLoad_triggered() {
    mFileName = QFileDialog::getOpenFileName(this, tr("Open EDF"), "./", tr("Cardiogram Files (*.edf)"));

     QLayoutItem* item;
     while ( (item = ui->verticalLayoutLeft->takeAt(0)) != nullptr) {
         item->widget()->deleteLater();
         delete item;
     }

    if(edfopen_file_readonly(mFileName.toLocal8Bit().data(), &mEDFHeader, EDFLIB_READ_ALL_ANNOTATIONS)) {
      QString errorString;
      switch(mEDFHeader.filetype)
      {
        case EDFLIB_MALLOC_ERROR                : errorString = "malloc error";
                                                  break;
        case EDFLIB_NO_SUCH_FILE_OR_DIRECTORY   : errorString = "can not open file, no such file or directory";
                                                  break;
        case EDFLIB_FILE_CONTAINS_FORMAT_ERRORS : errorString = "the file is not EDF(+) or BDF(+) compliant";

                                                  break;
        case EDFLIB_MAXFILES_REACHED            : errorString = "to many files opened";
                                                  break;
        case EDFLIB_FILE_READ_ERROR             : errorString = "a read error occurred";
                                                  break;
        case EDFLIB_FILE_ALREADY_OPENED         : errorString = "file has already been opened";
                                                  break;
        default                                 : errorString = "unknown error";
                                                  break;
      }
      ui->statusBar->showMessage(tr("Error: ") + errorString);
      return;
    }

    ui->statusBar->showMessage(tr("File: ") + QFileInfo(mFileName).fileName());

    printf("\nlibrary version: %i.%02i\n", edflib_version() / 100, edflib_version() % 100);

    printf("\ngeneral header:\n\n");

    printf("filetype: %i\n", mEDFHeader.filetype);
    printf("edfsignals: %i\n", mEDFHeader.edfsignals);
    printf("file duration: %lld seconds\n", mEDFHeader.file_duration / EDFLIB_TIME_DIMENSION);
    printf("startdate: %i-%i-%i\n", mEDFHeader.startdate_day, mEDFHeader.startdate_month, mEDFHeader.startdate_year);
    printf("starttime: %i:%02i:%02i\n", mEDFHeader.starttime_hour, mEDFHeader.starttime_minute, mEDFHeader.starttime_second);
    printf("patient: %s\n", mEDFHeader.patient);
    printf("recording: %s\n", mEDFHeader.recording);
    printf("patientcode: %s\n", mEDFHeader.patientcode);
    printf("gender: %s\n", mEDFHeader.gender);
    printf("birthdate: %s\n", mEDFHeader.birthdate);
    printf("patient_name: %s\n", mEDFHeader.patient_name);
    printf("patient_additional: %s\n", mEDFHeader.patient_additional);
    printf("admincode: %s\n", mEDFHeader.admincode);
    printf("technician: %s\n", mEDFHeader.technician);
    printf("equipment: %s\n", mEDFHeader.equipment);
    printf("recording_additional: %s\n", mEDFHeader.recording_additional);
    printf("datarecord duration: %f seconds\n", ((double)mEDFHeader.datarecord_duration) / EDFLIB_TIME_DIMENSION);

    printf("number of datarecords in the file: %lli\n", mEDFHeader.datarecords_in_file);
    printf("number of annotations in the file: %lli\n", mEDFHeader.annotations_in_file);

    printf("\nsignal parameters:\n\n");

    int channel = 0;

    printf("label: %s\n", mEDFHeader.signalparam[channel].label);
    printf("samples in file: %lli\n", mEDFHeader.signalparam[channel].smp_in_file);
    printf("samples in datarecord: %i\n", mEDFHeader.signalparam[channel].smp_in_datarecord);
    printf("physical maximum: %f\n", mEDFHeader.signalparam[channel].phys_max);
    printf("physical minimum: %f\n", mEDFHeader.signalparam[channel].phys_min);
    printf("digital maximum: %i\n", mEDFHeader.signalparam[channel].dig_max);
    printf("digital minimum: %i\n", mEDFHeader.signalparam[channel].dig_min);
    printf("physical dimension: %s\n", mEDFHeader.signalparam[channel].physdimension);
    printf("prefilter: %s\n", mEDFHeader.signalparam[channel].prefilter);
    printf("transducer: %s\n", mEDFHeader.signalparam[channel].transducer);
    printf("samplefrequency: %f\n", ((double)mEDFHeader.signalparam[channel].smp_in_datarecord / (double)mEDFHeader.datarecord_duration) * EDFLIB_TIME_DIMENSION);

    struct edf_annotation_struct annot;

    printf("\n");

    for(int i=0; i<mEDFHeader.annotations_in_file; i++) {
        if(edf_get_annotation(mEDFHeader.handle, i, &annot))
        {
            printf("\nerror: edf_get_annotations()\n");
            edfclose_file(mEDFHeader.handle);
            return;
        }
        else
        {
            printf("annotation: onset is %lli    duration is %s    description is %s\n",
                annot.onset / EDFLIB_TIME_DIMENSION,
                annot.duration,
                annot.annotation);
        }
    }
    mpGraphicAreaWidget->setEDFHeader(&mEDFHeader);

    double * pDoubleBuffer;

    for (int channel = 0; channel < mEDFHeader.edfsignals; channel++) {
        QString text = mEDFHeader.signalparam[channel].label;
        QLabel * pLabel = new QLabel(text.trimmed(), this);
        ui->verticalLayoutLeft->addWidget(pLabel);

        unsigned long long samplesCount = mEDFHeader.signalparam[channel].smp_in_file;

        pDoubleBuffer = (double *)malloc(samplesCount * sizeof(double));
        if(pDoubleBuffer == nullptr)
        {
            printf("\nmalloc error\n");
            edfclose_file(mEDFHeader.handle);
            return;
        }
        // установка позиции чтения
        edfseek(mEDFHeader.handle, channel, 0, EDFSEEK_SET);
        // чтение из файла
        int count = edfread_physical_samples(mEDFHeader.handle, channel, int(samplesCount), pDoubleBuffer);

        if ( count == (-1))
        {
          printf("\nerror: edf_read_physical_samples()\n");
          edfclose_file(mEDFHeader.handle);
          free(pDoubleBuffer);
          return;
        }

        mpGraphicAreaWidget->setData(quint32(channel), QByteArray((char*) pDoubleBuffer, int(samplesCount) * sizeof(double)));

        free(pDoubleBuffer);
    }
    edfclose_file(mEDFHeader.handle);
}
