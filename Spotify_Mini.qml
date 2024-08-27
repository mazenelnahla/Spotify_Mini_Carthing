import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import com.example.spotifyclient 1.0
// import UDPsender 1.0

Item {
    id: item1
    width: 1024
    height: 600
    property string trackName: "No playing song currently"
    property string artistName: "Spotify"
    property string albumName: ""
    property string albumImgUrl: ""
    property bool isPlaying // Property to track play/pause state
    property bool isPlaying2 // Property to track play/pause state



    Rectangle {
        id: background
        width: parent.width
        height: parent.height
        color: "#2E2F30"
        opacity: 1
        anchors.fill:parent
    }



    Image {
        id: backgroundRect
        source: albumImgUrl
        fillMode: Image.PreserveAspectCrop
        width: parent.width
        height: parent.height
        visible: true // Hide the image element
    }



    FastBlur {
        anchors.fill: backgroundRect
        source: backgroundRect
        radius: 100
    }



    SpotifyClient {
        id: spotifyClient
    }
    // UDPSender {
    //     id: udpSender
    // }



    Text {
        id: text1
        x: 849
        y: 577
        color: "#ffffff"
        text: qsTr("Powered by Mazen Elnahla")
        font.pixelSize: 13
        font.family: "Arial"
    }



    Text {
        id: text2
        x: 21
        y: 577
        color: "#ffffff"
        text: qsTr("Submodule of Auto Spectra")
        font.pixelSize: 13
        font.family: "Arial"
    }



    Image {
        id: spotify_logo
        source: "images/spotify_logo.png"
        sourceSize.height: 42
        sourceSize.width: 138
        fillMode: Image.PreserveAspectFit
        x: 45
        y: 60
        opacity: 1
    }



    Rectangle {
        id: artistImage
        radius: 15
        baselineOffset: 1
        visible: true
        color: "#000000"
        x: 38
        y: 125
        width: 353
        height: 353
        focus: true

        // Ensure ShaderEffectSource has correct sourceItem
        ShaderEffectSource {
            id: artistImageSource
            x: 0
            y: 0
            width: 353
            height: 353
            activeFocusOnTab: true
            focus: true
            antialiasing: true
            sourceItem: artistImage
            recursive: true
        }

        // Use a single Image with OpacityMask for rounded corners
        Image {
            x: 0
            y: 0
            source: albumImgUrl ? albumImgUrl : "qrc:/images/unknown_song.png"
            focus: true
            asynchronous: true
            fillMode: Image.Stretch
            width: 353
            height: 353
            opacity: 1
            layer.enabled: true
            layer.effect: OpacityMask {
                maskSource: artistImageSource
            }
        }
    }




    Text {
        id: album_name
        text: albumName
        font.pixelSize: 37
        font.bold: true
        color: "white"
        smooth: true
        x: 422
        y: 130
        width: 544
        height: 76
        opacity: 1
        style:
            Text.Outline;
            styleColor: "#212121"
    }




    Text {
        id: song_name
        text: trackName
        wrapMode: Text.WrapAnywhere
        font.pixelSize: 33
        verticalAlignment: Text.AlignVCenter
        font.bold: true
        color: "#ffffff"
        smooth: true
        x: 423
        y: 214
        width: 570
        height: 75
        opacity: 1
        style:
            Text.Outline;
            styleColor: "#212121"

    }



    Text {
        id: artist_name
        text: artistName
        font.pixelSize: 24
        font.bold: true
        color: "#ffffff"
        smooth: true
        x: 421
        y: 295
        opacity: 1
        style:
            Text.Outline;
            styleColor: "#212121"

    }




    ProgressBar {
        id:musicProgress
        x: 420
        y: 364
        value: 100
        from: 0
        to: 100
        width: 560
        height: 5
        background: Rectangle {
            implicitWidth: 200
            implicitHeight: 6
            color: "#7f7f7f"
            radius: 3
            opacity: 0.5
        }
        contentItem: Item {
            implicitWidth: 200
            implicitHeight: 4

            Rectangle {
                width: musicProgress.visualPosition * parent.width
                height: parent.height
                radius: 2
                color: "#ffffff"
            }
        }
        Text {
            id: current_time
            text: "00:00"
            font.pixelSize: 16
            font.family: "Arial"
            font.bold: true
            color: "#ffffff"
            smooth: true
            x: 0
            y: 15
            opacity: 1
            style:
                Text.Outline;
                styleColor: "#212121"

        }
        Text {
            id: finish_time
            text: "00:00"
            font.pixelSize: 16
            font.family: "Arial"
            font.bold: true
            color: "#ffffff"
            smooth: true
            x: 520
            y: 15
            opacity: 1
            style:
                Text.Outline;
                styleColor: "#212121"

        }
    }





    Item{
        id:media_control
        x:0
        y:420
        Image {
            id: play
            source: "images/play.png"
            x: 651
            y: -10
            opacity: isPlaying ? 0 : 1
        }
        Image {
            id: pause
            source: "images/pause.png"
            x: 651
            y: -10
            opacity: isPlaying ? 1 : 0
        }
        MouseArea {
            id: playPauseArea
            x: 651
            y: 0
            width: play.width
            height: play.height
            onClicked: {
                if(isPlaying2){
                    spotifyClient.pause()
                }else{
                    spotifyClient.play()
                }
                isPlaying2 = !isPlaying2;
            }
        }

        Image {
            id: forward
            source: "images/forward.png"
            x: 771
            y: 0
            opacity: 1
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    spotifyClient.nextTrack()
                }
            }
        }
        Image {
            id: back
            source: "images/back.png"
            x: 551
            y: 0
            opacity: 1

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    spotifyClient.previousTrack()
                }
            }
        }
    }




    Item{
        id:status_bar
        x:0
        y:0
        Rectangle{
            width:1024
            height: 35
            x:0
            y:0
            color:"#000000"
            opacity: 0.4
        }

        Text {
            id: clock
            text: "Clock"
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            font.pointSize: 12
            font.styleName: "Bold"
            font.family: "Arial"
            color: "#ffffff"
            x: 915
            y: 6
            width: 100
            height: 22
            opacity: 1
        }
        Text {
            id: dateText
            font.styleName: "Bold"
            font.family: "Arial"
            x: 10
            y: 6
            opacity: 1
            color: "#ffffff"
            text: "Date"
            font.pointSize: 12
            style:
                Text.Outline;
                styleColor: "#212121"

        }
        Timer {
            interval: 1000
            running: true
            repeat: true
            onTriggered: {
                clock.text = formatTimeWithoutAMPM(new Date()) +" "+ "<b>"+Qt.formatTime(new Date(), "AP")+"</b>"
            }
            function formatTimeWithoutAMPM(dateTime) {
                var hours = dateTime.getHours()
                var minutes = dateTime.getMinutes()

                // Convert to 12-hour format
                var ampm = hours >= 12 ? "PM" : "AM"
                hours = hours % 12
                hours = hours ? hours : 12  // Handle midnight (12:00 AM)

                // Add leading zeros
                hours = ("0" + hours).slice(-2)
                minutes = ("0" + minutes).slice(-2)
                return hours + ":" + minutes
            }
        }
        Timer {
            id: dateTimer
            interval: 1000 // 1 second interval
            running: true
            repeat: true
            onTriggered: {
                dateText.text=Qt.formatDateTime(new Date(), "dd "+"MMMM "+"yyyy")
            }
        }
    }



    Connections {
        target: spotifyReceiver
        function onSpotifyReceivedData(Track_Name, Artist_Name, Album_Name, Album_Img_URL,  is_Playing, currentTime, duration, currentTimeformatted, durationformatted) {
            trackName = Track_Name
            artistName = Artist_Name
            albumName = Album_Name
            albumImgUrl = Album_Img_URL
            isPlaying = is_Playing
            musicProgress.value = currentTime
            musicProgress.to = duration
            current_time.text = currentTimeformatted
            finish_time.text = durationformatted
            if (Track_Name === "") {
                trackName = "No playing song currently"
                artistName = "Spotify"
            }
        }
        function onIsConnectedChanged(status){
            lower_alert.visible=!status;
        }

    }
    Item{
        id:lower_alert
        y:540
        width: 573
        height: 186
        visible: false
        anchors.horizontalCenter: parent.horizontalCenter
        Image{
            id:alert_lower_bg
            y: -33
            width: 537
            height: 186
            visible: true
            source: "/images/lower_status.png"
            anchors.horizontalCenter: parent.horizontalCenter
            sourceSize.height: 186
            sourceSize.width: 600
            cache: true
            enabled: true
            smooth: true
            fillMode: Image.Stretch
        }
        Image{
            id:no_wifi_icon
            x: 57
            width: 50
            height: 50
            visible: true
            anchors.verticalCenter: parent.verticalCenter
            source:"/images/no-wifi_white.png"
            sourceSize.height: 50
            sourceSize.width: 50
            anchors.verticalCenterOffset: -75
            fillMode: Image.PreserveAspectFit
        }
        Text{
            x: 115
            y: 8
            width: 398
            height: 24
            color: "#aa0000"
            text: "there is no internet connection, please reconnect."
            font.letterSpacing: 1
            font.pixelSize: 20
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.WrapAnywhere
            font.wordSpacing: 0
            font.capitalization: Font.Capitalize
            font.styleName: "Black"
            font.family: "Arial"
        }
    }
}

/*##^##
Designer {
    D{i:0}D{i:4;locked:true}
}
##^##*/
