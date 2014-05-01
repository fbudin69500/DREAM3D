/* ============================================================================
 * Copyright (c) 2014 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2014 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
 #include "FilePathGenerator.h"

#include <QtCore/QDir>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilePathGenerator::FilePathGenerator(){
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilePathGenerator::~FilePathGenerator()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QString> FilePathGenerator::GenerateFileList(int start, int end, bool &hasMissingFiles,
                                                      bool stackLowToHigh,const QString& inputPath,
                                                      const QString& filePrefix,
                                                      const QString& fileSuffix, const QString& fileExtension,
                                                      int paddingDigits)
{
  QVector<QString> fileList;
    QDir dir(inputPath);
    if(dir.exists() == false)
    {
        return fileList;
    }
    int index = 0;

  QString filename;
  for (int i = 0; i < (end-start)+1; ++i)
  {
    if (stackLowToHigh)
    {
      index = start + i;
    }
    else
    {
      index = end - i;
    }
    filename = QString("%1%2%3.%4").arg(filePrefix)
        .arg(QString::number(index), paddingDigits, '0')
        .arg(fileSuffix).arg(fileExtension);
    QString filePath = inputPath + QDir::separator() + filename;
    filePath = QDir::toNativeSeparators(filePath);
    fileList.push_back(filePath);
  }
  return fileList;
}

