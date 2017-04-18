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
#include "ChatFilterTest.h"

using namespace Profile;

ChatFilterTest::ChatFilterTest(QObject *parent) :
    QObject(parent)
{
}

QVector<UserData*> ChatFilterTest::userFactory(QVector<QPair<QString, QString>> v) {
    int id = 1;
    QVector<UserData*> users;
    for(auto i = v.begin(); i != v.end(); i++, id++) {
        QString name = (*i).first;
        QString surname = (*i).second;
        auto ud = new UserData(id);
        // These fields are necessary to fill in order to prevent crash.
        ud->botInfo = std::make_unique<BotInfo>();
        ud->input = MTP_inputPeerUser(MTP_int(id), MTP_long(0));
        ud->inputUser = MTP_inputUser(MTP_int(id), MTP_long(0));
        ud->asUser()->setName(name, surname, "", name+surname);
        users.append(ud);
    }
    return users;
}

void ChatFilterTest::initTestCase() {
    // This is only necessary to init it's internal singleton pointer
    messenger = Messenger::InstancePointer();
    if (messenger == nullptr)
        messenger = new Messenger();
}

void ChatFilterTest::cleanupTestCase() {
    // Don't delete messenger here, otherwise it crashes!
}

void ChatFilterTest::filterByName() {
    // Create a chat
    PeerId newid = peerFromChat(101);
    ChatData* channel = new ChatData(newid);
    channel->input = MTP_inputPeerChat(MTP_int(newid));
    Q_ASSERT(nullptr != channel->asChat());

    QVector<UserData*> users = userFactory({
                                               qMakePair(QString("John"), QString("Doe")),
                                               qMakePair(QString("Mike"), QString("Jogers")),
                                               qMakePair(QString("Johnatan"), QString("Brown")),
                                               qMakePair(QString("Stephanie"), QString("Lorenz")),
                                               qMakePair(QString("123jodfsdatan"), QString("123123"))
                                           });
    for(auto i = users.begin(); i != users.end(); i++)
        channel->participants.insert(*i, (*i)->id);

    // Create the main user
    auto me = new UserData(0);
    me->asUser()->setName("Me", "Surname", "", "");
    App::setSelf(me);

    // Init the widget
    QWidget *window = new QWidget();
    auto membersWidget = new GroupMembersWidget(window, channel);

    // Test cases
    (*membersWidget->getFilter())->setText("Jo");
    membersWidget->onFilterUpdate();
    QCOMPARE(membersWidget->itemsCount(), 4);

    (*membersWidget->getFilter())->setText("mik");
    membersWidget->onFilterUpdate();
    QCOMPARE(membersWidget->itemsCount(), 1);

    (*membersWidget->getFilter())->setText("aTaN");
    membersWidget->onFilterUpdate();
    QCOMPARE(membersWidget->itemsCount(), 2);

    // Cleanup
    delete membersWidget;
    delete window;
    delete me;
    App::setSelf(nullptr);
    while(users.size() > 0) {
        delete users.at(0);
        users.pop_front();
    }
    delete channel;
}

void ChatFilterTest::filterBySurName() {
    // Create a chat
    PeerId newid = peerFromChat(102);
    ChatData* channel = new ChatData(newid);
    channel->input = MTP_inputPeerChat(MTP_int(newid));
    Q_ASSERT(nullptr != channel->asChat());

    QVector<UserData*> users = userFactory({
                                               qMakePair(QString("John"), QString("Doe")),
                                               qMakePair(QString("Mike"), QString("Brownie")),
                                               qMakePair(QString("Johnatan"), QString("Brown")),
                                               qMakePair(QString("Stephanie"), QString("Lorenz")),
                                               qMakePair(QString("123jodfsdatan"), QString("1231enz23"))
                                           });
    for(auto i = users.begin(); i != users.end(); i++)
        channel->participants.insert(*i, (*i)->id);

    // Create the main user
    auto me = new UserData(0);
    me->asUser()->setName("Me", "Surname", "", "");
    App::setSelf(me);

    // Init the widget
    QWidget *window = new QWidget();
    auto membersWidget = new GroupMembersWidget(window, channel);

    // Test cases
    (*membersWidget->getFilter())->setText("Doe");
    membersWidget->onFilterUpdate();
    QCOMPARE(membersWidget->itemsCount(), 1);

    (*membersWidget->getFilter())->setText("eNz");
    membersWidget->onFilterUpdate();
    QCOMPARE(membersWidget->itemsCount(), 2);

    (*membersWidget->getFilter())->setText("Brow");
    membersWidget->onFilterUpdate();
    QCOMPARE(membersWidget->itemsCount(), 2);

    // Cleanup
    delete membersWidget;
    delete window;
    delete me;
    App::setSelf(nullptr);
    while(users.size() > 0) {
        delete users.at(0);
        users.pop_front();
    }
    delete channel;
}
