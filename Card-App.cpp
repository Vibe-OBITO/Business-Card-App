#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFile>
#include <QTextStream>
#include <QImageReader>
#include <QByteArray>
#include <QPixmap>
#include <QByteArray>
#include <QDataStream>

class BusinessCard
{
public:
    BusinessCard(const QString& name, const QString& jobTitle, const QString& contactInfo, const QPixmap& photo)
        : name(name), jobTitle(jobTitle), contactInfo(contactInfo), photo(photo)
    {
    }

    QString getName() const { return name; }
    QString getJobTitle() const { return jobTitle; }
    QString getContactInfo() const { return contactInfo; }
    QPixmap getPhoto() const { return photo; }

private:
    QString name;  // Имя
    QString jobTitle;  // Должность
    QString contactInfo;  // Контактная информация
    QPixmap photo;  // Фото
};

class BusinessCardApp : public QWidget
{
    Q_OBJECT

public:
    BusinessCardApp(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setupUI();
    }

private slots:
    // Обработчик нажатия кнопки "Добавить визитку"
    void addBusinessCard()
    {
        QString name = nameLineEdit->text();
        QString jobTitle = jobTitleLineEdit->text();
        QString contactInfo = contactInfoLineEdit->text();

        // Проверка наличия введенных данных и выбранного фото
        if (name.isEmpty() || jobTitle.isEmpty() || contactInfo.isEmpty() || selectedPhoto.isNull())
        {
            QMessageBox::warning(this, "Ошибка", "Пожалуйста, заполните все поля и выберите фото.");
            return;
        }

        // Создание новой визитки
        BusinessCard businessCard(name, jobTitle, contactInfo, selectedPhoto);
        businessCards.append(businessCard);

        // Добавление визитки в список
        QListWidgetItem* item = new QListWidgetItem(name);
        businessCardListWidget->addItem(item);
    }

    // Обработчик нажатия кнопки "Выбрать фото"
    void selectPhoto()
    {
        QString fileName = QFileDialog::getOpenFileName(this, "Выбрать фото", "", "Изображения (*.png *.jpg *.jpeg)");
        if (!fileName.isEmpty())
        {
            selectedPhoto.load(fileName);
            photoLabel->setPixmap(selectedPhoto.scaled(100, 100));
        }
    }

    // Обработчик нажатия кнопки "Сохранить визитки"
    void saveBusinessCards()
    {
        QString fileName = QFileDialog::getSaveFileName(this, "Сохранить визитки", "", "Бинарные файлы (*.dat)");
        if (!fileName.isEmpty())
        {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly))
            {
                QDataStream out(&file);
                out << businessCards.size();
                for (const BusinessCard& businessCard : businessCards)
                {
                    out << businessCard.getName() << businessCard.getJobTitle() << businessCard.getContactInfo() << businessCard.getPhoto().toImage();
                }
                file.close();
                QMessageBox::information(this, "Сохранение визиток", "Визитки успешно сохранены.");
            }
            else
            {
                QMessageBox::critical(this, "Ошибка", "Не удалось сохранить визитки.");
            }
        }
    }

    // Обработчик нажатия кнопки "Загрузить визитки"
    void loadBusinessCards()
    {
        QString fileName = QFileDialog::getOpenFileName(this, "Загрузить визитки", "", "Бинарные файлы (*.dat)");
        if (!fileName.isEmpty())
        {
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly))
            {
                QDataStream in(&file);
                int size;
                in >> size;
                businessCards.clear();
                businessCardListWidget->clear();
                for (int i = 0; i < size; ++i)
                {
                    QString name, jobTitle, contactInfo;
                    QImage photo;
                    in >> name >> jobTitle >> contactInfo >> photo;
                    BusinessCard businessCard(name, jobTitle, contactInfo, QPixmap::fromImage(photo));
                    businessCards.append(businessCard);
                    QListWidgetItem* item = new QListWidgetItem(name);
                    businessCardListWidget->addItem(item);
                }
                file.close();
                QMessageBox::information(this, "Загрузка визиток", "Визитки успешно загружены.");
            }
            else
            {
                QMessageBox::critical(this, "Ошибка", "Не удалось загрузить визитки.");
            }
        }
    }

private:
    // Настройка пользовательского интерфейса
    void setupUI()
    {
        QVBoxLayout* layout = new QVBoxLayout(this);

        QLabel* nameLabel = new QLabel("Имя:");
        nameLineEdit = new QLineEdit(this);
        layout->addWidget(nameLabel);
        layout->addWidget(nameLineEdit);

        QLabel* jobTitleLabel = new QLabel("Должность:");
        jobTitleLineEdit = new QLineEdit(this);
        layout->addWidget(jobTitleLabel);
        layout->addWidget(jobTitleLineEdit);

        QLabel* contactInfoLabel = new QLabel("Контакты:");
        contactInfoLineEdit = new QLineEdit(this);
        layout->addWidget(contactInfoLabel);
        layout->addWidget(contactInfoLineEdit);

        QPushButton* selectPhotoButton = new QPushButton("Выбрать фото", this);
        connect(selectPhotoButton, &QPushButton::clicked, this, &BusinessCardApp::selectPhoto);
        layout->addWidget(selectPhotoButton);

        photoLabel = new QLabel(this);
        photoLabel->setFixedSize(100, 100);
        layout->addWidget(photoLabel);

        QPushButton* addButton = new QPushButton("Добавить визитку", this);
        connect(addButton, &QPushButton::clicked, this, &BusinessCardApp::addBusinessCard);
        layout->addWidget(addButton);

        QPushButton* saveButton = new QPushButton("Сохранить визитки", this);
        connect(saveButton, &QPushButton::clicked, this, &BusinessCardApp::saveBusinessCards);
        layout->addWidget(saveButton);

        QPushButton* loadButton = new QPushButton("Загрузить визитки", this);
        connect(loadButton, &QPushButton::clicked, this, &BusinessCardApp::loadBusinessCards);
        layout->addWidget(loadButton);

        businessCardListWidget = new QListWidget(this);
        layout->addWidget(businessCardListWidget);

        setLayout(layout);

        // Установка стилей для улучшения эстетики
        setStyleSheet(
            "QLabel { font-weight: bold; }"
            "QPushButton { padding: 8px 16px; }"
            "QListWidget { border: 1px solid #ccc; }"
        );
    }

    QLineEdit* nameLineEdit;
    QLineEdit* jobTitleLineEdit;
    QLineEdit* contactInfoLineEdit;
    QLabel* photoLabel;
    QListWidget* businessCardListWidget;
    QPixmap selectedPhoto;
    QVector<BusinessCard> businessCards;
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    BusinessCardApp businessCardApp;
    businessCardApp.show();

    return app.exec();
}

#include "main.moc"
