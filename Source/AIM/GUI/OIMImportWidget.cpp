/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "OIMImportWidget.h"

//-- Qt Includes
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QThread>
#include <QtCore/QFileInfoList>
#include <QtGui/QFileDialog>
#include <QtGui/QCloseEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QListWidget>

#include "AIM/Common/Constants.h"
#include "AIM/Common/Qt/QR3DFileCompleter.h"
#include "AIM/Common/Qt/AIM_QtMacros.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OIMImportWidget::OIMImportWidget(QWidget *parent) :
AIMPluginFrame(parent),
m_WorkerThread(NULL),
#if defined(Q_WS_WIN)
m_OpenDialogLastDirectory("C:\\")
#else
m_OpenDialogLastDirectory("~/")
#endif
{

  setupUi(this);
  setupGui();
  checkIOFiles();
}

OIMImportWidget::~OIMImportWidget()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OIMImportWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList) {
    w->setEnabled(b);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OIMImportWidget::setupGui()
{
  QR3DFileCompleter* com = new QR3DFileCompleter(this, true);
  oim_InputDir->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString &)),
           this, SLOT(on_oim_InputDir_textChanged(const QString &)));

  QR3DFileCompleter* com1 = new QR3DFileCompleter(this, false);
  oim_OutputFile->setCompleter(com1);
  QObject::connect( com1, SIGNAL(activated(const QString &)),
           this, SLOT(on_oim_OutputFile_textChanged(const QString &)));

  m_WidgetList << oim_InputDir << oim_InputDirBtn << oim_OutputFile << oim_OutputFileBtn;
  m_WidgetList << oim_FileExt << oim_ErrorMessage << oim_TotalDigits;
  m_WidgetList << oim_FilePrefix << oim_TotalSlices << oim_ZStartIndex << oim_ZEndIndex << oim_zSpacing;
  oim_ErrorMessage->setVisible(false);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OIMImportWidget::readSettings(QSettings &prefs)
{
  QString val;
  bool ok;
  qint32 i;
  //double d;

  prefs.beginGroup("OIMImport");
  READ_FILEPATH_SETTING(prefs, oim_InputDir, "");
  READ_STRING_SETTING(prefs, oim_FilePrefix, "");
  READ_STRING_SETTING(prefs, oim_FileSuffix, "");
  READ_STRING_SETTING(prefs, oim_FileExt, "ang");
//  READ_STRING_SETTING(prefs, oim_TotalSlices, "");
  READ_SETTING(prefs, oim_ZStartIndex, ok, i, 1 , Int);
  READ_SETTING(prefs, oim_ZEndIndex, ok, i, 10 , Int);
  READ_STRING_SETTING(prefs, oim_zSpacing, "0.25");
  READ_STRING_SETTING(prefs, oim_OutputFile, "Untitled.h5ang");
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OIMImportWidget::writeSettings(QSettings &prefs)
{
  prefs.beginGroup("OIMImport");
  WRITE_STRING_SETTING(prefs, oim_InputDir)
  WRITE_STRING_SETTING(prefs, oim_FilePrefix)
  WRITE_STRING_SETTING(prefs, oim_FileSuffix)
  WRITE_STRING_SETTING(prefs, oim_FileExt)
//  WRITE_STRING_SETTING(prefs, oim_TotalSlices)
  WRITE_STRING_SETTING(prefs, oim_ZStartIndex)
  WRITE_STRING_SETTING(prefs, oim_ZEndIndex)
  WRITE_STRING_SETTING(prefs, oim_zSpacing)
  WRITE_STRING_SETTING(prefs, oim_OutputFile)

  prefs.endGroup();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OIMImportWidget::on_oim_OutputFile_textChanged(const QString & text)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OIMImportWidget::checkIOFiles()
{
  if (true == this->verifyPathExists(oim_InputDir->text(), this->oim_InputDir))
   {
     oim_findAngMaxSliceAndPrefix();
   }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OIMImportWidget::on_oim_OutputFileBtn_clicked()
{
  QString file = QFileDialog::getSaveFileName(this, tr("Save OIM HDF5 File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("HDF5 OIM Files (*.h5ang)") );
  if ( true == file.isEmpty() ){ return;  }
  QFileInfo fi (file);
  QString ext = fi.suffix();
  oim_OutputFile->setText(fi.absoluteFilePath());
  m_OpenDialogLastDirectory = fi.path();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OIMImportWidget::on_oim_InputDirBtn_clicked()
{
  // std::cout << "on_angDirBtn_clicked" << std::endl;
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select Ang Directory"), outputFile);
  if (!outputFile.isNull())
  {
    this->oim_InputDir->setText(outputFile);
    oim_findAngMaxSliceAndPrefix();
    verifyPathExists(outputFile, oim_InputDir);
    m_OpenDialogLastDirectory = outputFile;
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OIMImportWidget::on_oim_InputDir_textChanged(const QString & text)
{
  if (verifyPathExists(oim_InputDir->text(), oim_InputDir) )
  {
    oim_findAngMaxSliceAndPrefix();
  }
  else
  {
    oim_FileListView->clear();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OIMImportWidget::on_oim_GoBtn_clicked()
{

  bool ok = false;
  if (oim_GoBtn->text().compare("Cancel") == 0)
  {
    if (m_OimImport != NULL)
    {
      emit cancelProcess();
    }
    return;
  }

  SANITY_CHECK_INPUT(oim_ , InputDir)

  if (m_WorkerThread != NULL)
  {
    m_WorkerThread->wait(); // Wait until the thread is complete
    delete m_WorkerThread; // Kill the thread
    m_WorkerThread = NULL;
  }
  m_WorkerThread = new QThread(); // Create a new Thread Resource

  m_OimImport = OIMImport::New();

  // Move the GrainGenerator object into the thread that we just created.
  m_OimImport->moveToThread(m_WorkerThread);

  m_OimImport->setOutputFile(oim_OutputFile->text().toStdString());
  m_OimImport->setZStartIndex(oim_ZStartIndex->value());
  m_OimImport->setZEndIndex(oim_ZEndIndex->value());
  m_OimImport->setZResolution(oim_zSpacing->text().toDouble(&ok));

  int fileCount = oim_FileListView->count();
  std::vector<std::string> fileList;
  for (int f = 0; f < fileCount; ++f)
  {
    fileList.push_back(oim_FileListView->item(f)->text().toStdString());
  }

  m_OimImport->setAngFileList(fileList);

  /* Connect the signal 'started()' from the QThread to the 'run' slot of the
   * Reconstruction object. Since the Reconstruction object has been moved to another
   * thread of execution and the actual QThread lives in *this* thread then the
   * type of connection will be a Queued connection.
   */
  // When the thread starts its event loop, start the Reconstruction going
  connect(m_WorkerThread, SIGNAL(started()), m_OimImport.get(), SLOT(compute()));

  // When the Reconstruction ends then tell the QThread to stop its event loop
  connect(m_OimImport.get(), SIGNAL(finished() ), m_WorkerThread, SLOT(quit()));

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()), this, SLOT( threadFinished() ));

  // Send Progress from the Reconstruction to this object for display
  connect(m_OimImport.get(), SIGNAL (updateProgress(int)), this, SLOT(threadProgressed(int) ));

  // Send progress messages from Reconstruction to this object for display
  connect(m_OimImport.get(), SIGNAL (updateMessage(QString)), this, SLOT(threadHasMessage(QString) ));

  // If the use clicks on the "Cancel" button send a message to the Reconstruction object
  // We need a Direct Connection so the
  connect(this, SIGNAL(cancelProcess() ), m_OimImport.get(), SLOT (on_CancelWorker() ), Qt::DirectConnection);

  setWidgetListEnabled(false);
  emit
  processStarted();
  m_WorkerThread->start();
  oim_GoBtn->setText("Cancel");

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OIMImportWidget::on_oim_ZEndIndex_valueChanged(int value)
{
  oim_generateExampleOimInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OIMImportWidget::on_oim_ZStartIndex_valueChanged(int value)
{
  oim_generateExampleOimInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OIMImportWidget::on_oim_TotalDigits_valueChanged(int value)
{
    oim_generateExampleOimInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OIMImportWidget::on_oim_FileExt_textChanged(const QString &string)
{
  oim_generateExampleOimInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OIMImportWidget::on_oim_FileSuffix_textChanged(const QString &string)
{
  oim_generateExampleOimInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OIMImportWidget::on_oim_FilePrefix_textChanged(const QString &string)
{
  oim_generateExampleOimInputFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OIMImportWidget::oim_generateExampleOimInputFile()
{

  QString filename = QString("%1%2%3.%4").arg(oim_FilePrefix->text())
      .arg(oim_ZStartIndex->text(), oim_TotalDigits->value(), '0')
      .arg(oim_FileSuffix->text()).arg(oim_FileExt->text());
  oim_GeneratedFileNameExample->setText(filename);

  // Now generate all the file names the user is asking for and populate the table
  oim_FileListView->clear();
  int start = oim_ZStartIndex->value();
  int end = oim_ZEndIndex->value();
  QIcon greenDot = QIcon(QString(":/green-dot.png"));
  QIcon redDot = QIcon(QString(":/red-dot.png"));
  bool hasMissingFiles = false;
  for (int i = start; i <= end; ++i)
  {
    filename = QString("%1%2%3.%4").arg(oim_FilePrefix->text())
        .arg(QString::number(i), oim_TotalDigits->value(), '0')
        .arg(oim_FileSuffix->text()).arg(oim_FileExt->text());
    QString filePath = oim_InputDir->text() + QDir::separator() + filename;
    QFileInfo fi(filePath);
    QListWidgetItem* item = new QListWidgetItem(filePath, oim_FileListView);
    if (fi.exists() == true)
    {
      item->setIcon(greenDot);
    }
    else
    {
      hasMissingFiles = true;
      item->setIcon(redDot);
    }
  }
  if (hasMissingFiles == true)
  {
    oim_ErrorMessage->setVisible(true);
    oim_ErrorMessage->setText("Alert: File(s) on the list do NOT exist on the filesystem. Please make sure all files exist");
  }
  else
  {
    oim_ErrorMessage->setVisible(false);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OIMImportWidget::oim_findAngMaxSliceAndPrefix()
{
  if (oim_InputDir->text().length() == 0) { return; }
  QDir dir(oim_InputDir->text());
  QStringList filters;
  QString ext = "." + oim_FileExt->text();
  filters << "*"+ext;
  dir.setNameFilters(filters);
  QFileInfoList angList = dir.entryInfoList();
  int minSlice = 0;
  int maxSlice = 0;
  int currValue = 0;
  bool flag = false;
  bool ok;
  QString fPrefix;
  QRegExp rx("(\\d+)");
  QStringList list;
  int pos = 0;
  int digitStart = 0;
  int digitEnd = 0;
  int totalOimFilesFound = 0;
  foreach(QFileInfo fi, angList)
  {
    if (fi.suffix().compare(ext) && fi.isFile() == true)
    {
      pos = 0;
      list.clear();
      QString fn = fi.baseName();
      digitStart = pos = rx.indexIn(fn, pos);
      digitEnd = digitStart;
      while ((pos = rx.indexIn(fn, pos)) != -1)
      {
        list << rx.cap(0);
        fPrefix = fn.left(pos);
        pos += rx.matchedLength();
      }
      while(digitEnd >= 0 && fn[digitEnd] >= '0' && fn[digitEnd]<='9')
      {
        ++digitEnd;
      }
      oim_TotalDigits->setValue(digitEnd - digitStart);
      if (list.size() > 0) {
        currValue = list.front().toInt(&ok);
        if (false == flag) { minSlice = currValue; flag = true;}
        if (currValue > maxSlice) { maxSlice = currValue; }
        if (currValue < minSlice) { minSlice = currValue; }
      }
      ++totalOimFilesFound;
    }
  }
  this->oim_TotalSlices->setText(QString::number(totalOimFilesFound));
  this->oim_FilePrefix->setText(fPrefix);
  this->oim_ZStartIndex->setValue(minSlice);
  this->oim_ZEndIndex->setValue(maxSlice);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OIMImportWidget::threadFinished()
{
 // std::cout << "OIMImportWidget::threadFinished()" << std::endl;
  oim_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->oim_progressBar->setValue(0);
  emit processEnded();
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OIMImportWidget::threadProgressed(int val)
{
  this->oim_progressBar->setValue( val );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OIMImportWidget::threadHasMessage(QString message)
{
  if (NULL != this->statusBar()) {
    this->statusBar()->showMessage(message);
  }
}
