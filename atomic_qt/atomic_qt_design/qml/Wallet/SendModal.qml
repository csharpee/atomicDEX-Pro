import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import "../Components"
import "../Constants"

// Open Enable Coin Modal
DefaultModal {
    id: root

    onClosed: if(stack_layout.currentIndex === 2) reset(true)

    // Local
    readonly property var default_prepare_send_result: ({ has_error: false, error_message: "", tx_hex: "", date: "", fees: "", explorer_url: "" })
    property var prepare_send_result: default_prepare_send_result
    property string send_result

    function isERC20() {
        return API.get().current_coin_info.type === "ERC-20"
    }

    function prepareSendCoin(address, amount, fee_enabled, fee_amount, is_erc_20, gas, gas_price) {
        let max = parseFloat(API.get().current_coin_info.balance) === parseFloat(amount)

        if(fee_enabled) {
            if(max === false && !is_erc_20)
                max = parseFloat(amount) + parseFloat(fee_amount) >= parseFloat(API.get().current_coin_info.balance)

            prepare_send_result = API.get().prepare_send_fees(address, amount, is_erc_20, fee_amount, gas_price, gas, max)
        }
        else {
            prepare_send_result = API.get().prepare_send(address, amount, max)
        }

        if(prepare_send_result.has_error) {
            text_error.text = prepare_send_result.error_message
        }
        else {
            text_error.text = ""

            // Change page
            stack_layout.currentIndex = 1
        }
    }

    function sendCoin() {
        send_result = API.get().send(prepare_send_result.tx_hex)
        stack_layout.currentIndex = 2
    }

    function reset(close = false) {
        prepare_send_result = default_prepare_send_result
        send_result = ""

        input_address.field.text = ""
        input_amount.field.text = ""
        input_custom_fees.field.text = ""
        input_custom_fees_gas.field.text = ""
        input_custom_fees_gas_price.field.text = ""
        custom_fees_switch.checked = false
        text_error.text = ""

        if(close) root.close()
        stack_layout.currentIndex = 0
    }

    function feeIsHigherThanAmount() {
        if(!custom_fees_switch.checked) return false

        const amt = parseFloat(input_amount.field.text)
        const fee_amt = parseFloat(input_custom_fees.field.text)

        return amt < fee_amt
    }

    function hasFunds() {
        if(!General.hasEnoughFunds(true, API.get().current_coin_info.ticker, "", "", input_amount.field.text))
            return false

        if(custom_fees_switch.checked) {
            if(isERC20()) {
                const ether = 1000000000
                const gas_limit = parseFloat(input_custom_fees_gas.field.text)
                const gas_price = parseFloat(input_custom_fees_gas_price.field.text)
                const fee_eth = (gas_limit * gas_price)/ether

                if(API.get().current_coin_info.ticker === "ETH") {
                    const amount = parseFloat(input_amount.field.text)
                    const total_needed_eth = amount + fee_eth
                    if(!General.hasEnoughFunds(true, "ETH", "", "", total_needed_eth.toString()))
                        return false
                }
                else {
                    if(!General.hasEnoughFunds(true, "ETH", "", "", fee_eth.toString()))
                        return false
                }
            }
            else {
                if(feeIsHigherThanAmount()) return false

                if(!General.hasEnoughFunds(true, API.get().current_coin_info.ticker, "", "", input_custom_fees.field.text))
                    return false
            }
        }

        return true
    }

    function fieldAreFilled() {
        return input_address.field.text != "" &&
             input_amount.field.text != "" &&
             input_address.field.acceptableInput &&
             input_amount.field.acceptableInput &&
             parseFloat(input_amount.field.text) > 0 &&
             (!custom_fees_switch.checked || (
                    (!isERC20() && input_custom_fees.field.acceptableInput) ||
                    (isERC20() && input_custom_fees_gas.field.acceptableInput && input_custom_fees_gas_price.field.acceptableInput &&
                                    parseFloat(input_custom_fees_gas.field.text) > 0 && parseFloat(input_custom_fees_gas_price.field.text) > 0)
                  )
              )
    }

    // Inside modal
    width: stack_layout.children[stack_layout.currentIndex].width + horizontalPadding * 2
    height: stack_layout.children[stack_layout.currentIndex].height + verticalPadding * 2
    StackLayout {
        id: stack_layout

        // Prepare Page
        ColumnLayout {
            Layout.fillWidth: true

            ModalHeader {
                title: API.get().empty_string + (qsTr("Prepare to Send"))
            }

            // Send address
            AddressField {
                id: input_address
                title: API.get().empty_string + (qsTr("Recipient's address"))
                field.placeholderText: API.get().empty_string + (qsTr("Enter address of the recipient"))
            }

            // Amount input
            AmountField {
                id: input_amount
                title: API.get().empty_string + (qsTr("Amount to send"))
                field.placeholderText: API.get().empty_string + (qsTr("Enter the amount to send"))
            }

            // Custom fees switch
            Switch {
                id: custom_fees_switch
                text: API.get().empty_string + (qsTr("Enable Custom Fees"))
                onCheckedChanged: input_custom_fees.field.text = ""
            }

            // Custom Fees section
            ColumnLayout {
                visible: custom_fees_switch.checked

                DefaultText {
                    font.pointSize: Style.textSize
                    color: Style.colorRed
                    text: API.get().empty_string + (qsTr("Only use custom fees if you know what you are doing!"))
                }

                // Normal coins, Custom fees input
                AmountField {
                    visible: !isERC20()

                    id: input_custom_fees
                    title: API.get().empty_string + (qsTr("Custom Fee") + " [" + API.get().current_coin_info.ticker + "]")
                    field.placeholderText: API.get().empty_string + (qsTr("Enter the custom fee"))
                }

                // ERC-20 coins
                ColumnLayout {
                    visible: isERC20()

                    // Gas input
                    AmountIntField {
                        id: input_custom_fees_gas
                        title: API.get().empty_string + (qsTr("Gas Limit") + " [Gwei]")
                        field.placeholderText: API.get().empty_string + (qsTr("Enter the gas limit"))
                    }

                    // Gas price input
                    AmountIntField {
                        id: input_custom_fees_gas_price
                        title: API.get().empty_string + (qsTr("Gas Price") + " [Gwei]")
                        field.placeholderText: API.get().empty_string + (qsTr("Enter the gas price"))
                    }
                }
            }


            // Fee is higher than amount error
            DefaultText {
                id: fee_error
                wrapMode: Text.Wrap
                visible: feeIsHigherThanAmount()

                color: Style.colorRed

                text: API.get().empty_string + (qsTr("Custom Fee can't be higher than the amount"))
            }

            // Not enough funds error
            DefaultText {
                wrapMode: Text.Wrap
                visible: !fee_error.visible && fieldAreFilled() && !hasFunds()

                color: Style.colorRed

                text: API.get().empty_string + (qsTr("Not enough funds.") + "\n" + qsTr("You have %1", "AMT TICKER").arg(General.formatCrypto("", API.get().get_balance(API.get().current_coin_info.ticker), API.get().current_coin_info.ticker)))
            }

            DefaultText {
                id: text_error
                color: Style.colorRed
                visible: text !== ''
            }

            // Buttons
            RowLayout {
                DefaultButton {
                    text: API.get().empty_string + (qsTr("Close"))
                    Layout.fillWidth: true
                    onClicked: root.close()
                }
                PrimaryButton {
                    text: API.get().empty_string + (qsTr("Prepare"))
                    Layout.fillWidth: true

                    enabled: fieldAreFilled() && hasFunds()

                    onClicked: prepareSendCoin(input_address.field.text, input_amount.field.text, custom_fees_switch.checked, input_custom_fees.field.text,
                                               isERC20(), input_custom_fees_gas.field.text, input_custom_fees_gas_price.field.text)
                }
            }
        }

        // Send Page
        ColumnLayout {
            ModalHeader {
                title: API.get().empty_string + (qsTr("Send"))
            }

            // Address
            TextWithTitle {
                title: API.get().empty_string + (qsTr("Recipient's address"))
                text: API.get().empty_string + (input_address.field.text)
            }

            // Amount
            TextWithTitle {
                title: API.get().empty_string + (qsTr("Amount"))
                text: API.get().empty_string + (General.formatCrypto("", input_amount.field.text, API.get().current_coin_info.ticker))
            }

            // Fees
            TextWithTitle {
                title: API.get().empty_string + (qsTr("Fees"))
                text: API.get().empty_string + (General.formatCrypto("", prepare_send_result.fees, API.get().current_coin_info.ticker))
            }

            // Date
            TextWithTitle {
                title: API.get().empty_string + (qsTr("Date"))
                text: API.get().empty_string + (prepare_send_result.date)
            }

            // Buttons
            RowLayout {
                DefaultButton {
                    text: API.get().empty_string + (qsTr("Back"))
                    Layout.fillWidth: true
                    onClicked: stack_layout.currentIndex = 0
                }
                PrimaryButton {
                    text: API.get().empty_string + (qsTr("Send"))
                    Layout.fillWidth: true
                    onClicked: sendCoin()
                }
            }
        }

        // Result Page
        SendResult {
            result: prepare_send_result
            address: input_address.field.text
            tx_hash: send_result
            custom_amount: input_amount.field.text

            function onClose() { reset(true) }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:600;width:1200}
}
##^##*/
