import sys
import socket
import struct
import os
from PyQt5.QtWidgets import (QApplication, QWidget, QDial, QLabel, QVBoxLayout, QSizePolicy,
                             QFrame, QPushButton, QMainWindow, QAction, QStackedWidget,
                             QToolBar, QMessageBox, QDialog, QHBoxLayout, QComboBox, QTextEdit)
from PyQt5.QtGui import QColor, QPalette
from PyQt5.QtCore import Qt, QThread, pyqtSignal, pyqtSlot


def send_can_message(iface, can_id, data):
    sock = None
    try:
        if os.geteuid() != 0:
            raise PermissionError("Root permissions are required to access the CAN interface.")

        sock = socket.socket(socket.AF_CAN, socket.SOCK_RAW, socket.CAN_RAW)
        sock.bind((iface,))
        can_frame_fmt = "=IB3x8s"
        can_dlc = len(data)
        can_packet = struct.pack(can_frame_fmt, can_id, can_dlc, data.ljust(8, b'\x00'))
        sock.send(can_packet)
        print(f"Sent message on {iface} with ID {hex(can_id)} and data {data.hex()}")
    except PermissionError as pe:
        print(f"Permission Error: {pe}")
    except OSError as oe:
        print(f"OS Error: {oe}")
    except Exception as e:
        print(f"Error: {e}")
    finally:
        if sock:
            sock.close()


def receive_can_message(iface, callback):
    try:
        sock = socket.socket(socket.AF_CAN, socket.SOCK_RAW, socket.CAN_RAW)
        sock.bind((iface,))
        can_frame_fmt = "=IB3x8s"
        while True:
            can_packet = sock.recv(16)
            can_id, can_dlc, data = struct.unpack(can_frame_fmt, can_packet)
            data = data[:can_dlc]
            callback(can_id, data)
    except OSError as e:
        print(f"Error receiving CAN message: {e}")
    finally:
        sock.close()


class DigitalGauge(QFrame):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setStyleSheet('background-color: #2c3e50; border: 8px solid #3498db; border-radius: 20px;')
        self.setLayout(QVBoxLayout())
        self.layout().setContentsMargins(10, 10, 10, 10)

        self.label = QLabel(self)
        self.label.setAlignment(Qt.AlignCenter)
        self.label.setStyleSheet('color: #ecf0f1; font-size: 24pt; font-weight: bold; border: none;')
        self.layout().addWidget(self.label)

        self._value = 0
        self.update_value()

    def set_value(self, value):
        self._value = value
        self.update_value()

    def update_value(self):
        self.label.setText(f'{self._value} km/h')


class AnalogDigitalGauge(QFrame):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setStyleSheet('background-color: #2c3e50; border: 16px solid #3498db; border-radius: 20px;')
        self.setLayout(QVBoxLayout())
        self.layout().setContentsMargins(10, 10, 10, 10)

        self.dial = QDial(self)
        self.dial.setNotchesVisible(True)
        self.dial.setRange(0, 260)
        self.dial.valueChanged[int].connect(self.valuechanged)

        dial_size = self.dial.sizeHint().width() * 4
        self.dial.setFixedSize(dial_size, dial_size)

        palette = QPalette()
        palette.setColor(QPalette.Window, QColor(0, 0, 0))
        palette.setColor(QPalette.WindowText, QColor(255, 255, 255))
        self.dial.setPalette(palette)

        self.digital_gauge = DigitalGauge(self)
        self.layout().addWidget(self.dial, 2, alignment=Qt.AlignCenter)
        self.layout().addWidget(self.digital_gauge, 1, alignment=Qt.AlignCenter)

        self.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.setFixedSize(900, 800)

        self.digital_gauge.set_value(self.dial.value())

    def valuechanged(self, value):
        self.digital_gauge.set_value(value)


class MainWidget(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.analog_digital_gauge = AnalogDigitalGauge(self)

        palette = QPalette()
        palette.setColor(QPalette.Window, QColor(0, 0, 0))
        palette.setColor(QPalette.WindowText, QColor(255, 255, 255))
        self.setPalette(palette)

        layout = QVBoxLayout(self)
        layout.addWidget(self.analog_digital_gauge)


class CustomMessageBox(QMessageBox):
    def __init__(self, *args, **kwargs):
        super(CustomMessageBox, self).__init__(*args, **kwargs)
        self.setStyleSheet('''
            QMessageBox {
                background-color: #ffffff;
                color: #ecf0f1;
                border: none;
                border-radius: 0px;
            }
            QMessageBox QPushButton {
                background-color: #3498db;
                color: #ecf0f1;
                padding: 0px;
                border: none;
                border-radius: 0px;
            }
            QMessageBox QPushButton:hover {
                background-color: #2980b9;
                border: none;
                border-radius: 0px;
            }
            QMessageBox QPushButton:pressed {
                background-color: #1abc9c;
                border: none;
                border-radius: 0px;
            }
        ''')


class AdditionalPage(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.initUI()
        self.setStyleSheet('background-color: #2c3e50; border: 8px solid #3498db; border-radius: 20px;')

    def initUI(self):
        if self.parent() is not None:
            self.setStyleSheet('background-color: #2c3e50; border: 8px solid #3498db; border-radius: 20px;')
            self.setLayout(QVBoxLayout())
            self.label = QLabel(self)
            self.label.setAlignment(Qt.AlignCenter)
            self.label.setStyleSheet('color: #ecf0f1; font-size: 24pt; font-weight: bold; border: none;')

            btn_check_updates = QPushButton("Download Update")
            btn_run_diagnosis = QPushButton("Run Diagnosis")
            btn_exit = QPushButton("Exit")

            btn_check_updates.setStyleSheet("font-size: 22pt; padding: 50px; color: white;")
            btn_run_diagnosis.setStyleSheet("font-size: 22pt; padding: 50px; color: white;")
            btn_exit.setStyleSheet("font-size: 22pt; padding: 50px; color: white;")

            self.layout().addWidget(btn_check_updates)
            self.layout().addWidget(btn_run_diagnosis)
            self.layout().addWidget(btn_exit)

            btn_run_diagnosis.clicked.connect(self.runDiagnosisDialog)
            btn_check_updates.clicked.connect(self.showDownloadConfirmation)
            btn_exit.clicked.connect(self.goBackToMainWidget)

    def showDownloadConfirmation(self):
        reply = CustomMessageBox.question(self, 'Download Update',
                                          "Are you sure you want to download the latest update?",
                                          QMessageBox.Yes | QMessageBox.No, QMessageBox.No)

        if reply == QMessageBox.Yes:
            self.sendUpdateRequest()

    def sendUpdateRequest(self):
        send_can_message('can0', 0x123, b'\x11')
        print("Update request sent")

        self.listener_thread = CANListener(self.handleUpdateResponse)
        self.listener_thread.start()

    def handleUpdateResponse(self, can_id, data):
        if can_id == 0x123 and data == b'\x12':
            self.showUpdateDone()

    def runDiagnosisDialog(self):
        dialog = DiagnosisDialog(self)
        dialog.exec()

    def runDiagnosis(self, option):
        if option == "DTC by Status Mask":
            self.runDTCStatusMaskDialog()
        elif option == "Read DTC Snapshot Identifier":
            self.runDTCSnapshotIdentifierDialog()
        elif option == "Read DTC Snapshot Data by DTC Number":
            self.runDTCDataByNumberDialog()

    def runDTCStatusMaskDialog(self):
        dialog = DTCStatusMaskDialog(self)
        dialog.exec()

    def runDTCSnapshotIdentifierDialog(self):
        dialog = DTCSnapshotIdentifierDialog(self)
        dialog.exec()

    def runDTCDataByNumberDialog(self):
        dialog = DTCDataByNumberDialog(self)
        dialog.exec()

    def goBackToMainWidget(self):
        self.parent().setCurrentIndex(0)


class CANListener(QThread):
    message_received = pyqtSignal(int, bytes)

    def __init__(self, parent=None):
        super().__init__(parent)
        self.parent = parent

    def run(self):
        def callback(can_id, data):
            self.message_received.emit(can_id, data)

        receive_can_message('can0', callback)


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.stacked_widget = QStackedWidget()
        self.setCentralWidget(self.stacked_widget)

        self.main_widget = MainWidget()
        self.additional_page = AdditionalPage(self.stacked_widget)

        self.stacked_widget.addWidget(self.main_widget)
        self.stacked_widget.addWidget(self.additional_page)

        self.createMenu()

    def createMenu(self):
        options_toolbar = QToolBar("Options")
        self.addToolBar(Qt.TopToolBarArea, options_toolbar)
        self.setStyleSheet('background-color: #2c3e50; border-radius: 20px; color: white;')

        action_additional_page = QAction("Additional Page", self)
        action_additional_page.triggered.connect(self.showAdditionalPage)

        options_toolbar.addAction(action_additional_page)

    def showAdditionalPage(self):
        self.stacked_widget.setCurrentWidget(self.additional_page)
        self.resize(self.additional_page.sizeHint())

    def showMessage(self, can_id, data):
        dlg = CustomMessageBox(self)
        dlg.setWindowTitle("Message Received")
        dlg.setText(f"CAN ID: {hex(can_id)}, Data: {data.hex()}")
        dlg.setStandardButtons(QMessageBox.Ok)
        dlg.exec()


class DiagnosisDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Run Diagnosis")
        self.setLayout(QVBoxLayout())

        self.setFixedSize(parent.size())

        btn_dtc_status_mask = QPushButton("Run DTC by Status Mask")
        btn_dtc_snapshot_id = QPushButton("Read DTC Snapshot Identifier")
        btn_dtc_snapshot_data = QPushButton("Read DTC Snapshot Data by DTC Number")

        button_style = "font-size: 22pt; padding: 50px; color: white;"
        btn_dtc_status_mask.setStyleSheet(button_style)
        btn_dtc_snapshot_id.setStyleSheet(button_style)
        btn_dtc_snapshot_data.setStyleSheet(button_style)

        btn_dtc_status_mask.clicked.connect(lambda: self.runDiagnosisOption("DTC by Status Mask"))
        btn_dtc_snapshot_id.clicked.connect(lambda: self.runDiagnosisOption("Read DTC Snapshot Identifier"))
        btn_dtc_snapshot_data.clicked.connect(lambda: self.runDiagnosisOption("Read DTC Snapshot Data by DTC Number"))

        self.layout().addWidget(btn_dtc_status_mask)
        self.layout().addWidget(btn_dtc_snapshot_id)
        self.layout().addWidget(btn_dtc_snapshot_data)

    def runDiagnosisOption(self, option):
        self.parent().runDiagnosis(option)
        self.accept()


class DTCStatusMaskDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("DTC by Status Mask")
        self.setLayout(QVBoxLayout())

        self.listener = CANListener()
        self.listener.message_received.connect(self.displayMessage)

        btn_read_all_dtc = QPushButton("Read All DTC")
        btn_read_failed_dtc = QPushButton("Read Failed DTC")

        btn_read_all_dtc.clicked.connect(self.read_all_dtc)
        btn_read_failed_dtc.clicked.connect(self.read_failed_dtc)

        self.layout().addWidget(btn_read_all_dtc)
        self.layout().addWidget(btn_read_failed_dtc)

    def read_all_dtc(self):
        send_can_message('can0', 0x125, b'\x21')
        print("Read All DTC message sent")
        self.listener.start()

    def read_failed_dtc(self):
        send_can_message('can0', 0x125, b'\x22')
        print("Read Failed DTC message sent")
        self.listener.start()

    @pyqtSlot(int, bytes)
    def displayMessage(self, can_id, data):
        self.listener.quit()
        self.parent().showMessage(can_id, data)
        self.accept()


class DTCSnapshotIdentifierDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Read DTC Snapshot Identifier")
        self.setLayout(QVBoxLayout())

        self.listener = CANListener()
        self.listener.message_received.connect(self.displayMessage)

        btn_read_dtc_snapshot_id = QPushButton("Read DTC Snapshot Identifier")

        btn_read_dtc_snapshot_id.clicked.connect(self.read_dtc_snapshot_identifier)

        self.layout().addWidget(btn_read_dtc_snapshot_id)

    def read_dtc_snapshot_identifier(self):
        send_can_message('can0', 0x125, b'\x23')
        print("Read DTC Snapshot Identifier message sent")
        self.listener.start()

    @pyqtSlot(int, bytes)
    def displayMessage(self, can_id, data):
        self.listener.quit()
        self.parent().showMessage(can_id, data)
        self.accept()


class DTCDataByNumberDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Read DTC Snapshot Data by DTC Number")
        self.setLayout(QVBoxLayout())

        self.listener = CANListener()
        self.listener.message_received.connect(self.displayMessage)

        btn_dtc1 = QPushButton("Choose DTC1")
        btn_dtc2 = QPushButton("Choose DTC2")

        btn_dtc1.clicked.connect(lambda: self.read_dtc_data(1))
        btn_dtc2.clicked.connect(lambda: self.read_dtc_data(2))

        self.layout().addWidget(btn_dtc1)
        self.layout().addWidget(btn_dtc2)

    def read_dtc_data(self, dtc_number):
        if dtc_number == 1:
            send_can_message('can0', 0x125, b'\x24')
            print("Read DTC1 Data message sent")
        elif dtc_number == 2:
            send_can_message('can0', 0x125, b'\x25')
            print("Read DTC2 Data message sent")
        self.listener.start()

    @pyqtSlot(int, bytes)
    def displayMessage(self, can_id, data):
        self.listener.quit()
        self.parent().showMessage(can_id, data)
        self.accept()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    mainWindow = MainWindow()
    mainWindow.setWindowTitle('Analog Gauge')
    mainWindow.resize(1000, 700)

    mainWindow.show()
    sys.exit(app.exec_())
