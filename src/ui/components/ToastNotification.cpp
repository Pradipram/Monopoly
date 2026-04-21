#include "ToastNotification.h"

#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTimer>

ToastNotification::ToastNotification(const QString& text, const QColor& bgColor, QWidget* parent)
    : QLabel(text, parent)
{
    // 1. Determine text color based on background lightness
    // If the color is very bright (like Yellow or Cyan), use dark gray text. Otherwise, white.
    QString textColor = "white";
    if (bgColor == Qt::yellow || bgColor == Qt::cyan) {
        textColor = "#333333";
    }

    // 2. Format the Stylesheet
    QString style =
        QString(
            "background-color: rgba(%1, %2, %3, 220);"  // 220 alpha for slight transparency
            "color: %4;"
            "padding: 12px 24px;"
            "border-radius: 8px;"
            "font-size: 16px;"
            "font-weight: bold;"
            "border: 2px solid rgba(0, 0, 0, 40);"  // Subtle dark border to make it pop
            )
            .arg(bgColor.red())
            .arg(bgColor.green())
            .arg(bgColor.blue())
            .arg(textColor);

    this->setStyleSheet(style);
    this->setAlignment(Qt::AlignCenter);
    this->adjustSize();  // Auto-size to fit the text exactly

    // 3. Position the toast in the top-right corner with margins
    if (parent) {
        int xPos = parent->width() - this->width() - 20;  // Right side with 20px margin
        int yPos = 20;                                    // Top with 20px margin
        this->move(xPos, yPos);
    }

    this->hide();  // Keep it hidden until showToast() is called
}

void ToastNotification::showToast()
{
    this->raise();  // Ensure it floats on top of everything
    this->show();

    // 1. Setup the opacity effect
    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(effect);

    // 2. Fade In Animation (300ms)
    QPropertyAnimation* fadeIn = new QPropertyAnimation(effect, "opacity", this);
    fadeIn->setDuration(kFadeInDurationMs);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);

    // 3. Fade Out Animation (500ms)
    QPropertyAnimation* fadeOut = new QPropertyAnimation(effect, "opacity", this);
    fadeOut->setDuration(kFadeOutDurationMs);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);

    // 4. Start fading in, then wait 2 seconds and fade out
    fadeIn->start(QAbstractAnimation::DeleteWhenStopped);

    QTimer::singleShot(kFadeInDurationMs + kVisibleDurationMs, this, [fadeOut, this]() {
        fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
        // Automatically delete the toast from memory once it goes invisible!
        connect(fadeOut, &QPropertyAnimation::finished, this, &QLabel::deleteLater);
    });
}