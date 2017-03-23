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
#include "boxes/dialogsortbox.h"

#include "lang.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/buttons.h"
#include "storage/localstorage.h"
#include "boxes/confirmbox.h"
#include "mainwidget.h"
#include "mainwindow.h"
#include "langloaderplain.h"
#include "styles/style_boxes.h"

void DialogSortModeBox::prepare() {
    addButton(lang(lng_box_ok), [this] { saveDialogSortMode(); closeBox(); });

    setTitle(lang(lng_settings_dialog_sort_mode));

    auto y = st::boxOptionListPadding.top();
    _sortModes.reserve(modes.size());

    for(auto i = modes.constBegin(); i != modes.constEnd(); i++) {
        _sortModes.push_back(new Ui::Radiobutton(this, qsl("sortmode"), (int32)i.value(), i.key(), (int32)i.value() == cSortMode(), st::langsButton));
        _sortModes.back()->move(st::boxPadding.left() + st::boxOptionListPadding.left(), y);
        y += _sortModes.back()->heightNoMargins() + st::boxOptionListSkip;
    }

    auto optionsCount = modes.size();
    setDimensions(st::langsWidth, st::boxOptionListPadding.top() + optionsCount * st::langsButton.height + (optionsCount - 1) * st::boxOptionListSkip + st::boxOptionListPadding.bottom() + st::boxPadding.bottom());
}

void DialogSortModeBox::restoreDialogSortMode() {
    for (auto i = 0, l = _sortModes.size(); i != l; ++i) {
        if (_sortModes[i]->val() == cSortMode()) {
            _sortModes[i]->setChecked(true);
        }
    }
}

void DialogSortModeBox::saveDialogSortMode() {
    for (auto i = 0, l = _sortModes.size(); i != l; ++i) {
        if (_sortModes[i]->checked() && _sortModes[i]->val() != cSortMode()) {
            cSetSortMode(_sortModes[i]->val());
            Local::writeSettings();
            // TODO: update dialog widget dynamically
            App::restart();
        }
    }
}
