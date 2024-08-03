import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Styles 1.4


Window {
    id: window
    height: 600
    visible: true
    width: 1024
    color: "#3e3c3c"
    maximumHeight: 600
    maximumWidth: 1024
    minimumHeight: 600
    minimumWidth: 1024
    title: qsTr("Spotify Mini")
     // visibility: "FullScreen"
//     screen: Qt.application.primaryScreen
    Spotify_Mini{

    }
}








