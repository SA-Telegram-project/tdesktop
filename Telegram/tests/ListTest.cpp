/*
This file is part of Telegram Desktop,
the official desktop version of Telegram messaging app, see https://telegram.org

Telegram Desktop is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

It is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

In addition, as a special exception, the copyright holders give permission
to link the code of portions of this program with the OpenSSL library.

Full license: https://github.com/telegramdesktop/tdesktop/blob/master/LICENSE
Copyright (c) 2014-2017 John Preston, https://desktop.telegram.org
*/

#include <QTest>
#include "ListTest.h"

using namespace Dialogs;

ListTest::ListTest(QObject *parent) :
    QObject(parent)
{
}

void ListTest::sendMessagesToUser(List* dialog, PeerId id, int32 count) {
    Row* row = dialog->getRow(id);
    row->history()->lastMsgDate = QDateTime::currentDateTime();
    row->history()->_unreadCount = count;
    row->history()->updateChatListSortPosition();
    dialog->adjustByPos(row);
}

QVector<History*> ListTest::historyFactory() {
    PeerId ids[] = {1, 2, 3, 4, 5};
    QString names[] = {QString("Mike"), QString("John"), QString("Adam"), QString("Alex"), QString("Bob")};
    QVector<History*> history;
    history.reserve(5);
    for(int i = 0; i < 5; i++) {
        history.append(new History(ids[i]));
        history.at(i)->peer = new UserData(ids[i]);
        history.at(i)->peer->updateNameDelayed(names[i], names[i], names[i]);
        history.at(i)->peer->fillNames();
    }
    return history;
}

void ListTest::checkConsistency(List* dialog) {
    // Ensure that all elements exist and there are no duplicates
    QCOMPARE(dialog->size(), 5);
    int expected[] = {0, 0, 0, 0, 0};
    for(auto i = dialog->cbegin(); i != dialog->cend(); i++)
        expected[(*i)->history()->peer->id - 1]++;

    for(int i = 0; i < 5; i++)
        QCOMPARE(expected[i], 1);
}

void ListTest::cleanup(QVector<History*> history, List* dialog) {
    for(int i = 0; i < 5; i++) {
        History* h = history.at(i);
        if (h && h->peer)
            delete h->peer;
        if (h)
            delete h;
    }
    if (dialog)
        delete dialog;
}

void ListTest::sortByDate() {
    // 1. Initialize our dialogs
    QVector<History*> history = historyFactory();
    List* dialog = new List(SortMode::Date);
    for(int i = 0; i < 5; i++)
        dialog->addToEnd(history.at(i));

    checkConsistency(dialog);

    // 2. Simulate sending new messages
    // Send messages to user 5, 1, 2, 4 and 3
    sendMessagesToUser(dialog, 5, 2);
    sendMessagesToUser(dialog, 1, 1);
    sendMessagesToUser(dialog, 2, 3);
    sendMessagesToUser(dialog, 4, 4);
    sendMessagesToUser(dialog, 3, 2);
    checkConsistency(dialog);

    // 3. Check the order
    PeerId expected[] = {3, 4, 2, 1, 5};
    int pos = 0;
    for(auto i = dialog->cbegin(); i != dialog->cend(); i++, pos++)
        QCOMPARE((*i)->history()->peer->id, expected[pos]);

    // 4. Release resources
    cleanup(history, dialog);
}

void ListTest::sortByDateWithPinned() {
    // 1. Initialize our dialogs
    QVector<History*> history = historyFactory();
    List* dialog = new List(SortMode::Date);
    for(int i = 0; i < 5; i++)
        dialog->addToEnd(history.at(i));

    // Make some of dialogs pinned
    dialog->getRow(3)->history()->_pinnedIndex = 2; // Make user 3 pinned
    dialog->getRow(5)->history()->_pinnedIndex = 1; // Make user 5 pinned
    checkConsistency(dialog);

    // 2. Simulate sending new messages
    // Send messages to user 5, 1, 2, 4 and 3
    sendMessagesToUser(dialog, 5, 2);
    sendMessagesToUser(dialog, 1, 1);
    sendMessagesToUser(dialog, 2, 3);
    sendMessagesToUser(dialog, 4, 4);
    sendMessagesToUser(dialog, 3, 2);
    checkConsistency(dialog);

    // 3. Check the order
    PeerId expected[] = {3, 5, 4, 2, 1};
    int pos = 0;
    for(auto i = dialog->cbegin(); i != dialog->cend(); i++, pos++)
        QCOMPARE((*i)->history()->peer->id, expected[pos]);

    // 4. Release resources
    cleanup(history, dialog);
}

void ListTest::sortWithUnreadFirst() {
    // 1. Initialize our dialogs
    QVector<History*> history = historyFactory();
    List* dialog = new List(SortMode::UnreadFirst);
    for(int i = 0; i < 5; i++)
        dialog->addToEnd(history.at(i));

    checkConsistency(dialog);

    // 2. Simulate sending new messages
    // Send messages to user 5, 1, 2, 4 and 3. Mark some of them as unread.
    sendMessagesToUser(dialog, 5, 2);
    sendMessagesToUser(dialog, 1, 0);
    sendMessagesToUser(dialog, 2, 3);
    sendMessagesToUser(dialog, 4, 0);
    sendMessagesToUser(dialog, 3, 2);
    checkConsistency(dialog);

    // 3. Check the order: unread should be first!
    PeerId expected[] = {3, 2, 5, 4, 1};
    int pos = 0;
    for(auto i = dialog->cbegin(); i != dialog->cend(); i++, pos++)
        QCOMPARE((*i)->history()->peer->id, expected[pos]);

    // 4. Release resources
    cleanup(history, dialog);
}

void ListTest::sortWithUnreadFirstWithPinned() {
    // 1. Initialize our dialogs
    QVector<History*> history = historyFactory();
    List* dialog = new List(SortMode::UnreadFirst);
    for(int i = 0; i < 5; i++)
        dialog->addToEnd(history.at(i));

    // Make some of dialogs pinned
    dialog->getRow(3)->history()->_pinnedIndex = 2; // Make user 3 pinned
    dialog->getRow(5)->history()->_pinnedIndex = 1; // Make user 5 pinned
    checkConsistency(dialog);

    // 2. Simulate sending new messages
    // Send messages to user 5, 1, 2, 4 and 3. Mark some of them as unread.
    sendMessagesToUser(dialog, 5, 2);
    sendMessagesToUser(dialog, 1, 2);
    sendMessagesToUser(dialog, 2, 0);
    sendMessagesToUser(dialog, 4, 1);
    sendMessagesToUser(dialog, 3, 0);
    checkConsistency(dialog);

    // 3. Check the order: unread should be first!
    PeerId expected[] = {3, 5, 4, 1, 2};
    int pos = 0;
    for(auto i = dialog->cbegin(); i != dialog->cend(); i++, pos++)
        QCOMPARE((*i)->history()->peer->id, expected[pos]);

    // 4. Release resources
    cleanup(history, dialog);
}
