import QtQuick 1.1
import com.nokia.meego 1.0

PageStackWindow {
    id: appWindow

    showStatusBar: false

    initialPage: mainPage

    MainPage {
        id: mainPage
    }

    ToolBarLayout {
        id: commonTools
        visible: false
    }
}
