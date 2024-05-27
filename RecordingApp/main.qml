import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.15

Window {
    id: mainWindow;
    title: qsTr("Audio Text Analyzer")

    visible: true
    visibility: Window.FullScreen;

    Rectangle{
        id: rect_recoderHeader;
        width: parent.width;
        height: parent.height * 0.15;
        color: "transparent";

        anchors{
            top: parent.top;
            left: parent.left;
        }

        Text{
            id: headerTitle;
            text: "Audio Text Analyzer";
            anchors.centerIn: parent;
            color: "black";
            visible: false;
            font{
                pixelSize: parent.height * 0.5;
                bold: true;
            }
        }

        Image{
            id: image_closeBtn;
            source: "qrc:/closeBtn.png";
            width: parent.height * 0.85;
            height: parent.height * 0.85;
            anchors{
                top: parent.top;
                topMargin: parent.height * 0.1;
                right: parent.right;
                rightMargin: parent.height * 0.25;
            }

            cache: true;
            visible: true;
            fillMode: Image.PreserveAspectFit

            MouseArea{
                anchors.fill: parent;
                onClicked: {
//                    AudioApp.stopAudioRecording();
                    Qt.quit();
                }
            }
        }
    }
    Rectangle{
        id: rect_recoderBody;
        width: parent.width * 0.85;
        height: parent.height * 0.8;
        color: "transparent";
        anchors.centerIn: parent;

        Text{
            id: headerTitleBody;
            text: "Audio Text Analyzer";
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top;

            color: "black";
            font{
                pixelSize: parent.height * 0.15;
                bold: true;
            }
        }

        Rectangle{
            id: pauseRecordingBtn;
            width: parent.width * 0.15;
            height: parent.height * 0.3;
            anchors{
                left: parent.left;
                leftMargin: parent.width * 0.27;
                bottom: parent.bottom;
            }

            Image {
                id: image_pauseRecordingBtn;
                source: "qrc:/pause-button.png";
                width: parent.width * 0.5;
                height: parent.height * 0.5;
                anchors.centerIn: parent;
                cache: true;
                visible: true;
                fillMode: Image.PreserveAspectFit
            }
            MouseArea{
                anchors.fill: parent;
                onClicked: {
                    console.log("pauseAudioRecording");
                    AudioApp.pauseAudioRecording();
                    shaderEffect.isRunning = false;
                }
            }
        }

        Rectangle{
            id: startRecordingBtn;
            width: parent.width * 0.15;
            height: parent.height * 0.3;
            anchors{
                left: pauseRecordingBtn.right;
                bottom: parent.bottom;
            }

            Image {
                id: image_startRecordingBtn;
                source: "qrc:/rec-button.png";
                width: parent.width * 0.8;
                height: parent.height * 0.8;
                anchors.centerIn: parent;
                cache: true;
                visible: true;
                fillMode: Image.PreserveAspectFit
            }
            MouseArea{
                anchors.fill: parent;
                onClicked: {
                    console.log("startAudioRecording");
                    AudioApp.startAudioRecording();
                    shaderEffect.isRunning = true;
                    recordingAnimation.visible = true;
                }
            }
        }


        Rectangle{
            id: stopRecordingBtn;
            width: parent.width * 0.15;
            height: parent.height * 0.3;
            anchors{
                left: startRecordingBtn.right;
                bottom: parent.bottom;
            }

            Image {
                id: image_stopRecordingBtn;
                source: "qrc:/stop-button.png";
                width: parent.width * 0.5;
                height: parent.height * 0.5;
                anchors.centerIn: parent;
                cache: true;
                visible: true;
                fillMode: Image.PreserveAspectFit
            }
            MouseArea{
                anchors.fill: parent;
                onClicked: {
                    console.log("stopAudioRecording");
                    AudioApp.stopAudioRecording();
                    shaderEffect.isRunning = false;
                    recordingAnimation.visible = false;
                }
            }
        }
    }

    Image {
        id: sourceImg
        width: 1000;
        height: 150;
        anchors.centerIn: parent;
        source: "qrc:/SolidLine.png"
        visible: false;

    }

    Rectangle{
        id: recordingAnimation;
        width: sourceImg.width;
        height: sourceImg.height
        x: sourceImg.x;
        y: sourceImg.y;
        visible: false;
        color: "transparent"

        ShaderEffect {
            id: shaderEffect
            anchors.fill: parent
            property variant source: sourceImg
            property real frequency: 2.0
            property real amplitude: 0.45
            property real time: 0.0
            property bool isRunning: true

            NumberAnimation on time {
                from: 0; to: Math.PI*2; duration: 1000; loops: Animation.Infinite
                running: shaderEffect.isRunning;
            }
            fragmentShader: "
                            varying highp vec2 qt_TexCoord0;
                            uniform sampler2D source;
                            uniform lowp float qt_Opacity;
                            uniform highp float frequency;
                            uniform highp float amplitude;
                            uniform highp float time;
                            void main() {
                                highp vec2 texCoord = qt_TexCoord0;
                                texCoord.y = amplitude * sin(time * frequency + texCoord.x * 6.283185) + texCoord.y;
                                gl_FragColor = texture2D(source, texCoord) * qt_Opacity;
                            }"
        }
    }


    Rectangle{
        id: rect_recoderFooter;
        width: parent.width;
        height: parent.height * 0.1;
        color: "transparent";

        anchors{
            left: parent.left;
            bottom: parent.bottom;
        }

        Rectangle{
            id: rect_FooterLabel;
            height: parent.height;
            width: parent.width * 0.1;
            color: "red";

            anchors{
                left: parent.left;
                top: parent.top;
            }

            Text {
                id: text_FooterLabel;
                text: qsTr("Reply");
                anchors.centerIn: parent;
                color: "white";

                font{
                    pixelSize: parent.height * 0.5;
                    bold: true;
                }
            }
        }

        Rectangle {
            id: rect_FooterValue;
            width: parent.width * 0.9;
            height: parent.height;
            color: "transparent";
            anchors {
                left: rect_FooterLabel.right;
                top: parent.top;
            }

            border {
                width: 2;
                color: "red";
            }

            property string footerMessage: AudioApp.analyzedMessage

            Text {
                id: text_FootValue;
                text: rect_FooterValue.footerMessage
                color: "black";
                width: parent.width;
                height: parent.height;
                font.pixelSize: rect_FooterValue.height * 0.4;
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.centerIn: parent;
                wrapMode: Text.WordWrap;
            }

            Connections{
                target: AudioApp;
                onAnalyzedMessageChanged:{
                    rect_FooterValue.footerMessage = AudioApp.analyzedMessage;
                }
            }
        }
    }
}

