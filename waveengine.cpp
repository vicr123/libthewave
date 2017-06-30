#include "waveengine.h"
#include <QDateTime>
#include <QTimeZone>

WaveEngine::WaveEngine(QObject* parent) : QObject(parent)
{

}

void WaveEngine::processString(QString input) {
    switch (currentPane) {
        case None: {
            //Reset pane stage
            paneStage = 0;

            //Split query into words
            QString lcInput = input.toLower();
            QStringList words = lcInput.split(" ");

            //Look at each word from left to right
            for (int i = 0; i < words.count(); i++) {
                QString currentWord = words.at(i);
                if (currentWord == "email") {
                    processEmailString(input);
                    return;
                } else if (currentWord == "calculate") {
                    processCalculationString(input);
                    return;
                } else if (currentWord == "remind" || currentWord == "reminder") {
                    processReminderString(input);
                    return;
                } else if (currentWord == "hello" || currentWord == "hi") {
                    outputString("Hello! For help and tips, just ask \"What can you do?\"");
                    outputSpeech("Hello! For help and tips, just ask What can you do?");
                    return;
                } else if (currentWord == "time") {
                    processWhatTimeString(input);
                    return;
                } else if (currentWord == "help") {
                    changePane(Help);
                    outputString("Here are some things you can try");
                    outputSpeech("Here are some things you can try");

                    PaneProperties properties;
                    properties.title = "Help";
                    properties.buttonTitle = "View Help";
                    properties.buttonSpeechAction = "Help";
                    properties.buttonIcon = "help-contents";
                    outputInlinePane(properties);
                    return;
                }
            }

            if (lcInput.contains("what can you do")) {
                changePane(Help);
                outputString("Here are some things you can try");
                outputSpeech("Here are some things you can try");

                PaneProperties properties;
                properties.title = "Help";
                properties.buttonTitle = "View Help";
                properties.buttonSpeechAction = "Help";
                properties.buttonIcon = "help-contents";
                outputInlinePane(properties);
                return;
            } else if (lcInput.contains("silence")) {
                return;
            }

            outputString("Not sure what you're trying to get me to do.");
            outputSpeech("Not sure what you're trying to get me to do.");
            break;
        }
        case Email: {
            //Jump straight to email routine
            processEmailString(input);
            return;
        }
        case Reminder: {
            //Jump straight to reminder routine
            processReminderString(input);
        }
    }

}

WaveEngine::Pane WaveEngine::getCurrentPane() {
    return currentPane;
}

void WaveEngine::cancelCurrentPane() {
    if (currentPane == Email) {
        processString("Cancel");
    }
}

void WaveEngine::okCurrentPane() {
    if (currentPane == Email) {
        processString("send");
    }
}

void WaveEngine::processEmailString(QString input) {
    if (input.toLower() == "cancel") {
        //Cancel everything
        changePane(None);
        currentPane = None;

        outputString("Ok, let's forget that happened.");
        outputSpeech("Ok, let's forget that happened.");

        PaneProperties properties;
        properties.title = "Email Cancelled";
        properties.buttonTitle = "Compose another email";
        properties.buttonSpeechAction = "Send an email";
        properties.buttonIcon = "mail-send";
        outputInlinePane(properties);
    } else {
        auto doConfirmMessage = [=] {
            outputString("This is what I've got. Send, cancel, edit or add?");
            outputSpeech("This is what I've got. Send, cancel, edit or add?", true);
            currentPaneProperties.canOk = true;
            paneStage = 4;
        };

        switch (paneStage) {
            case 0: { //Nothing
                currentPaneProperties.field1 = "";
                currentPaneProperties.field2 = "";
                currentPaneProperties.field3 = "";
                currentPaneProperties.canOk = false;

                changePane(Email);
                currentPane = Email;

                outputString("Who's the recipient of this email?");
                outputSpeech("Who's the recipient of this email?", true);
                paneStage = 1;
                break;
            }
            case 1: { //Recipient
                currentPaneProperties.field1 = input;
                currentPaneProperties.canOk = false;

                outputString("What's the subject?");
                outputSpeech("What's the subject?", true);
                paneStage = 2;
                break;
            }
            case 2: { //Subject
                currentPaneProperties.field2 = input;
                currentPaneProperties.canOk = false;

                outputString("What's the message?");
                outputSpeech("What's the message?", true);
                paneStage = 3;
                break;
            }
            case 3: { //Body
                currentPaneProperties.field3 = input;
                currentPaneProperties.canOk = false;

                doConfirmMessage();
                break;
            }
            case 4: { //Confirmation
                if (input.toLower().contains("edit")) {
                    outputString("Recipient, Subject or message?");
                    outputSpeech("Recipient, Subject or message?", true);
                    currentPaneProperties.canOk = false;
                    paneStage = 5;
                } else if (input.toLower().contains("add")) {
                    outputString("What would you like to add to the message?");
                    outputSpeech("What would you like to add to the message?", true);
                    currentPaneProperties.canOk = false;
                    paneStage = 9;
                } else if (input.toLower().contains("send") || input.toLower().contains("ok") || input.toLower().contains("yes")) {
                    //Send message

                    changePane(None);
                    currentPane = None;

                    outputString("Ok, I've sent the message.");
                    outputSpeech("Ok, I've sent the message.");

                    PaneProperties properties;
                    properties.title = "Email";
                    properties.field1 = "To: " + currentPaneProperties.field1;
                    properties.field2 = "Subject: " + currentPaneProperties.field2;
                    properties.buttonTitle = "Compose another email";
                    properties.buttonSpeechAction = "Send an email";
                    properties.buttonIcon = "mail-send";
                    outputInlinePane(properties);
                }
                break;
            }
            case 5: { //Edit
                if (input.toLower().contains("recipient")) {
                    outputString("Who's the recipient of this email?");
                    outputSpeech("Who's the recipient of this email?", true);
                    currentPaneProperties.canOk = false;
                    paneStage = 6;
                } else if (input.toLower().contains("subject")) {
                    outputString("What's the subject?");
                    outputSpeech("What's the subject?", true);
                    currentPaneProperties.canOk = false;
                    paneStage = 7;
                } else if (input.toLower().contains("message")) {
                    outputString("What's the message?");
                    outputSpeech("What's the message?", true);
                    currentPaneProperties.canOk = false;
                    paneStage = 8;
                } else {
                    outputString("Recipient, Subject or message?");
                    outputSpeech("Recipient, Subject or message?", true);
                    currentPaneProperties.canOk = false;
                }
                break;
            }
            case 6: { //Edit Recipient
                currentPaneProperties.field1 = input;
                doConfirmMessage();
                break;
            }
            case 7: { //Edit Subject
                currentPaneProperties.field2 = input;
                doConfirmMessage();
                break;
            }
            case 8: { //Edit Body
                currentPaneProperties.field3 = input;
                doConfirmMessage();
                break;
            }
            case 9: { //Append to Body
                currentPaneProperties.field3 += input;
                doConfirmMessage();
                break;
            }
        }

        changePaneProperties(currentPaneProperties);
    }
}

void WaveEngine::processReminderString(QString input) {
    if (input.toLower() == "cancel") {
        //Cancel everything
        changePane(None);
        currentPane = None;

        outputString("Ok, let's forget that happened.");
        outputSpeech("Ok, let's forget that happened.");

        PaneProperties properties;
        properties.title = "Reminder Cancelled";
        properties.buttonTitle = "Set another reminder";
        properties.buttonSpeechAction = "Set a reminder";
        properties.buttonIcon = "chronometer";
        outputInlinePane(properties);
    } else {
        auto doConfirmMessage = [=] {
            outputString("Do you want to set this reminder?");
            outputSpeech("Do you want to set this reminder?", true);
            currentPaneProperties.canOk = true;
            paneStage = 3;
        };

        switch (paneStage) {
            case 0: { //Nothing
                currentPaneProperties.field1 = "";
                currentPaneProperties.field2 = "";
                currentPaneProperties.field3 = "";
                currentPaneProperties.canOk = false;

                changePane(Reminder);
                currentPane = Reminder;

                outputString("What do you want to be reminded about?");
                outputSpeech("What do you want to be reminded about?", true);
                paneStage = 1;
                break;
            }
            case 1: { //Reminder
                currentPaneProperties.field1 = input;
                currentPaneProperties.canOk = false;

                outputString("When do you want to be reminded?");
                outputSpeech("When do you want to be reminded?", true);
                paneStage = 2;
                break;
            }
            case 2: { //Time
                currentPaneProperties.field2 = input;
                currentPaneProperties.canOk = false;

                doConfirmMessage();
                break;
            }
            case 3: { //Confirmation
                if (input.toLower().contains("edit")) {
                    outputString("Reminder or time?");
                    outputSpeech("Reminder or time?", true);
                    currentPaneProperties.canOk = false;
                    paneStage = 4;
                } else if (input.toLower().contains("yes") || input.toLower().contains("ok") || input.toLower().contains("set")) {
                    //Set reminder message

                    changePane(None);
                    currentPane = None;

                    outputString("Ok, I'll remind you.");
                    outputSpeech("Ok, I'll remind you.");

                    PaneProperties properties;
                    properties.title = "Reminder";
                    properties.field1 = currentPaneProperties.field1;
                    properties.field2 = "At " + currentPaneProperties.field2;
                    properties.buttonTitle = "Set another reminder";
                    properties.buttonSpeechAction = "Set a reminder";
                    properties.buttonIcon = "chronometer";
                    outputInlinePane(properties);
                }
                break;
            }
            case 4: { //Edit
                if (input.toLower().contains("reminder")) {
                    outputString("What do you want to be reminded about?");
                    outputSpeech("What do you want to be reminded about?", true);
                    currentPaneProperties.canOk = false;
                    paneStage = 5;
                } else if (input.toLower().contains("time")) {
                    outputString("When do you want to be reminded?");
                    outputSpeech("When do you want to be reminded?", true);
                    currentPaneProperties.canOk = false;
                    paneStage = 6;
                } else {
                    outputString("Reminder or time?");
                    outputSpeech("Reminder or time?", true);
                    currentPaneProperties.canOk = false;
                }
                break;
            }
            case 5: { //Edit Reminder
                currentPaneProperties.field1 = input;
                doConfirmMessage();
                break;
            }
            case 6: { //Edit Time
                currentPaneProperties.field2 = input;
                doConfirmMessage();
                break;
            }
        }

        changePaneProperties(currentPaneProperties);
    }
}

void WaveEngine::processCalculationString(QString input) {
    if (QFile("/usr/bin/thecalculator").exists()) {
        QString numbersAndOperations = input;
        numbersAndOperations.remove("calculate");
        numbersAndOperations.remove("what is");
        numbersAndOperations.remove("what's");
        numbersAndOperations.replace("add", "+");
        numbersAndOperations.replace("plus", "+");
        numbersAndOperations.replace("minus", "-");
        numbersAndOperations.replace("subtract", "-");
        numbersAndOperations.replace("negative", "-");
        numbersAndOperations.replace("times", "*");
        numbersAndOperations.replace("multiply", "*");
        numbersAndOperations.replace("over", "/");
        numbersAndOperations.replace("divided by", "/");
        numbersAndOperations.replace("divide", "/");
        numbersAndOperations.replace("exponent", "^");
        numbersAndOperations.replace("to the power of", "^");
        numbersAndOperations.replace("squared", "^2");
        numbersAndOperations.replace("cubed", "^3");
        numbersAndOperations.replace("factorial", "!");

        QStringList numberReplacer = numbersAndOperations.split(" ");
        float currentNumber = 0;
        int startIndex = -1;
        bool doingPoints = false;
        for (QString part : numberReplacer) {
            bool isNum;
            part.toFloat(&isNum);
            if (isNum) {
            } else if (part == "point") {
                if (startIndex != -1) {
                    numberReplacer.replace(startIndex, QString::number(currentNumber));
                    for (int i = startIndex + 1; i < numberReplacer.indexOf(part); i++) {
                        numberReplacer.replace(i, "");
                    }
                    currentNumber = 0;
                }
                numberReplacer.replace(numberReplacer.indexOf("point"), ".");
                startIndex = numberReplacer.indexOf(part + 1);
                doingPoints = true;
            } else {
                if (startIndex != -1) {
                    numberReplacer.replace(startIndex, QString::number(currentNumber));
                    for (int i = startIndex + 1; i < numberReplacer.indexOf(part); i++) {
                        numberReplacer.replace(i, "");
                    }
                    currentNumber = 0;
                    startIndex = -1;
                }
                doingPoints = false;
            }
        }

        if (startIndex != -1) {
            numberReplacer.replace(startIndex, QString::number(currentNumber));
            for (int i = startIndex + 1; i < numberReplacer.count(); i++) {
                numberReplacer.replace(i, "");
            }
        }

        numberReplacer.removeAll("");
        numbersAndOperations = numberReplacer.join("");
        numbersAndOperations.replace(" . ", ".");

        QString displayExpression = numbersAndOperations;
        displayExpression.replace("*", "×");
        displayExpression.replace("/", "÷");
        displayExpression.replace(" ^ 2", "²");
        displayExpression.replace(" ^ 3", "³");
        displayExpression.replace(" ^ - 1", "⁻¹");
        displayExpression.replace(" ^ - 2", "⁻²");
        displayExpression.replace(" ^ - 3", "⁻³");
        displayExpression.replace(" !", "!");
        displayExpression.append(" =");

        //Call on theCalculator to do the heavy work ;)
        QProcess calc;
        calc.start("thecalculator -e \"" + numbersAndOperations + "\"");
        calc.setProcessChannelMode(QProcess::MergedChannels);
        calc.waitForFinished(-1);
        QByteArray answer = calc.readAll();
        bool hasErrorOccurred = calc.exitCode() == 2;

        if (hasErrorOccurred) {
            //emit changePane(Calculator);
            //currentPane = Calculator;

            PaneProperties properties;
            properties.title = "Calculator";
            properties.field1 = displayExpression;
            properties.field2 = answer;
            emit outputInlinePane(properties);
        } else {
            QString speakText = answer;
            speakText.replace("e+", " times 10 to the power of ");
            speakText.replace("e-", " times 10 to the power of negative ");
            if (speakText.left(1) == "-") {
                speakText.replace(0, 1, "negative ");
            }

            //emit changePane(Calculator);
            //currentPane = Calculator;

            PaneProperties properties;
            properties.title = "Calculator";
            properties.field1 = displayExpression;
            properties.field2 = answer;
            emit outputInlinePane(properties);

            emit outputSpeech(speakText);
        }
    } else {
        emit outputString("You'll need to install theCalculator to perform calculations using theWave.");
        emit outputSpeech("You'll need to install theCalculator to perform calculations using theWave.");
    }
}

void WaveEngine::processWhatTimeString(QString input) {
    QStringList list = input.split(" ");
    QString loc = "";
    QDateTime time = QDateTime::currentDateTime();

    if (list.contains("in")) {
        int indx = list.indexOf("in");
        list = list.mid(indx + 1);

        if (list.join(" ").toLower().contains("pago pago") || list.join(" ").toLower().contains("american samoa")) {
            loc = "Pago Pago, American Samoa";
            time.setTimeZone(QTimeZone(QString("Pacific/Pago_Pago").toUtf8()));
        } else if (list.join(" ").contains("niue")) {
            loc = "Niue";
            time.setTimeZone(QTimeZone(QString("Pacific/Niue").toUtf8()));
        } else {
            emit outputString("I'm not sure if you specified a place for me to check the time.");
            emit outputSpeech("I'm not sure if you specified a place for me to check the time.");
            return;
        }

        PaneProperties properties;
        properties.title = "Time in " + loc;
        properties.field1 = time.time().toString("h:mm AP");

        QString outputtime = time.time().toString("h:mm AP");
        outputtime.replace(":00", " o' clock");
        outputtime.replace(":0", " o ");

        emit outputInlinePane(properties);
        emit outputString("The time in " + loc + " is " + time.time().toString("h:mm AP") + ".");
        emit outputSpeech("The time in " + loc + " is " + outputtime);
    } else {
        emit outputString("I'm not sure if you specified a place for me to check the time.");
        emit outputSpeech("I'm not sure if you specified a place for me to check the time.");
    }
}
