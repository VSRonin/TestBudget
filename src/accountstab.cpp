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
#include "accountstab.h"
#include "ui_accountstab.h"
#include "addaccountdialog.h"
#include "relationaldelegate.h"
#include <QMessageBox>
#include <QHeaderView>
#include <mainobject.h>
#include "ownerdelegate.h"
AccountsTab::AccountsTab(QWidget *parent)
    : QWidget(parent)
    , m_object(nullptr)
    , ui(new Ui::AccountsTab)
    , m_currencyDelegate(new RelationalDelegate(this))
    , m_accountTypeDelagate(new RelationalDelegate(this))
    , m_ownerDelegate(new OwnerDelegate(this))
{
    ui->setupUi(this);
    ui->accountsView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(ui->addAccountButton, &QPushButton::clicked, this, &AccountsTab::onAddAccount);
    connect(ui->removeAccountButton, &QPushButton::clicked, this, &AccountsTab::onRemoveAccount);
}

AccountsTab::~AccountsTab()
{
    delete ui;
}

void AccountsTab::setMainObject(MainObject *mainObj)
{
    m_object = mainObj;
    if (m_object) {
        ui->accountsView->setModel(m_object->accountsModel());
        const auto setupView = [this]() {
            ui->accountsView->setColumnHidden(MainObject::acId, true);
            ui->accountsView->setItemDelegateForColumn(MainObject::acCurrency, m_currencyDelegate);
            ui->accountsView->setItemDelegateForColumn(MainObject::acAccountType, m_accountTypeDelagate);
            ui->accountsView->setItemDelegateForColumn(MainObject::acOwner, m_ownerDelegate);
        };
        connect(m_object->accountsModel(), &QAbstractItemModel::rowsInserted, this, setupView);
        connect(m_object->accountsModel(), &QAbstractItemModel::modelReset, this, setupView);
        setupView();
    } else
        ui->removeAccountButton->setEnabled(false);
    m_ownerDelegate->setMainObject(m_object);
    m_currencyDelegate->setRelationModel(m_object ? m_object->currenciesModel() : nullptr, MainObject::ccId, MainObject::ccCurrency);
    m_accountTypeDelagate->setRelationModel(m_object ? m_object->accountTypesModel() : nullptr, MainObject::atcId, MainObject::atcName);
    connect(ui->accountsView->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            [this]() { ui->removeAccountButton->setEnabled(!ui->accountsView->selectionModel()->selectedIndexes().isEmpty()); });
}

void AccountsTab::onAddAccount()
{
    Q_ASSERT(m_object);
    AddAccountDialog addAccountDialog(this);
    addAccountDialog.setMainObject(m_object);
    while (addAccountDialog.exec()) {
        if (m_object->addAccount(addAccountDialog.name(), addAccountDialog.owner(), addAccountDialog.curr(), addAccountDialog.typ()))
            break;
        QMessageBox::critical(this, tr("Error"), tr("Failed to add a new account, try again or check your input"));
    }
}

void AccountsTab::onRemoveAccount()
{
    QList<int> idsToRemove;
    const auto seletctedIndexes = ui->accountsView->selectionModel()->selectedIndexes();
    for (auto &&idx : seletctedIndexes)
        idsToRemove.append(idx.sibling(idx.row(), MainObject::acId).data().toInt());
    std::sort(idsToRemove.begin(), idsToRemove.end());
    idsToRemove.erase(std::unique(idsToRemove.begin(), idsToRemove.end()), idsToRemove.end());
    if (QMessageBox::question(this, tr("Are you sure?"),
                              tr("Are you sure you want to remove the selected account(s) and all related transactions?", "", idsToRemove.size()),
                              QMessageBox::Yes | QMessageBox::No)
        == QMessageBox::No)
        return;
    Q_ASSERT(m_object);
    if (!m_object->removeAccounts(idsToRemove))
        QMessageBox::critical(this, tr("Error"), tr("Failed to remove account(s), try again later", "", idsToRemove.size()));
}