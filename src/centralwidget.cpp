/****************************************************************************\
   Copyright 2024 Luca Beldi
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
       http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
\****************************************************************************/

#include "centralwidget.h"
#include "ui_centralwidget.h"
#include <mainobject.h>

CentralWidget::CentralWidget(QWidget *parent)
    : QWidget(parent)
    , m_object(nullptr)
    , ui(new Ui::CentralWidget)
{

    ui->setupUi(this);
}

CentralWidget::~CentralWidget()
{
    delete ui;
}

void CentralWidget::setMainObject(MainObject *mainObj)
{
    m_object = mainObj;
    ui->accountsWidget->setMainObject(mainObj);
    ui->transactionsWidget->setMainObject(mainObj);
    ui->familyWidget->setMainObject(mainObj);
}
