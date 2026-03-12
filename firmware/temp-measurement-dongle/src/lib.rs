#![no_std]

slint::slint! {
    export component TmbApp inherits Window {
        width: 320px;
        height: 172px;

        Rectangle {
            background: blue;

            Text {
                text: "Hello ESP32!";
                color: white;
                font-size: 24px;
                horizontal-alignment: center;
                vertical-alignment: center;
            }
        }
    }
}

extern crate alloc;

pub mod platforms;
