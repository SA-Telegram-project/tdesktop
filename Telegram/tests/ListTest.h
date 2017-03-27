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
#pragma once

#include "dialogs/dialogs_list.h"
#include <QObject>

using namespace Dialogs;

class ListTest: public QObject {
    Q_OBJECT
public:
    explicit ListTest(QObject *parent = 0);
private:
    QVector<History*> historyFactory();
    void sendMessagesToUser(List* dialog, PeerId id, int32 count);
    void checkConsistency(List* dialog);
    void cleanup(QVector<History*> history, List* dialog);
private slots:
    void sortByDate();
    void sortByDateWithPinned();
    void sortWithUnreadFirst();
    void sortWithUnreadFirstWithPinned();
};
