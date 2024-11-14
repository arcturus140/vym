#ifndef CONFLUENCEAGENT_H
#define CONFLUENCEAGENT_H

#include <QHash>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QObject>
#include <QTimer>

#include "confluence-user.h"

class BranchItem;
class VymModel;

//////////////////////////////////////////////////////////////////////////

class ConfluenceAgent : public QObject {
    Q_OBJECT

  public:
    enum JobType {
        Undefined,
        GetPageDetails,
        CreatePage,
        UpdatePage,
        UploadAttachments,
        GetUserInfo
    };

    static bool available();

    ConfluenceAgent();
    ConfluenceAgent(BranchItem *bi);
    ~ConfluenceAgent();
    void init();
    void setJobType(JobType jt);
    void setBranch(BranchItem *bi);
    void setModelID(uint id);
    void setPageURL(const QString &u);
    void setNewPageName(const QString &t);
    void setUploadPagePath(const QString &fp);
    void addUploadAttachmentPath(const QString &fp);

    void startJob();

  private:
    void continueJob(int nextStep = -1);
    void finishJob();
    void unknownStepWarningFinishJob();

  signals:
    void foundUsers(QList <ConfluenceUser>);

  public:
    void getUsers(const QString &name); //! Convenience function to get user data

  private: QNetworkRequest createRequest(const QUrl &url);
  private: void startGetPageSourceRequest(QUrl requestedUrl);
  private slots: void pageSourceReceived(QNetworkReply *reply);

  private: void startGetPageDetailsRequest();
  private slots: void pageDetailsReceived(QNetworkReply *reply);

  private: void startCreatePageRequest();
  private: void startUpdatePageRequest();
  private slots: void pageUploaded(QNetworkReply *reply);


  private: void startGetUserInfoRequest();
  private slots: void userInfoReceived(QNetworkReply *reply);

  private: void startGetAttachmentsInfoRequest();
  private slots: void attachmentsInfoReceived(QNetworkReply *reply);

  private: void startCreateAttachmentRequest();
  private slots: void attachmentCreated(QNetworkReply *reply);

  private: void startUpdateAttachmentRequest();
  private slots: void attachmentUpdated(QNetworkReply *reply);

  signals:
    void attachmentsSuccess();
    void attachmentsFailure();

  public slots:
    void attachmentsUploadSuccess();
    void attachmentsUploadFailure();

  private: bool wasRequestSuccessful(
            QNetworkReply *reply, 
            const QString &requestDesc,
            const QByteArray &fullReply);


  private slots: void timeout();

#ifndef QT_NO_SSL
    void sslErrors(QNetworkReply *, const QList<QSslError> &errors);
#endif

  private:
    // Job related 
    QTimer *killTimer;
    JobType jobType;
    int jobStep;
    bool abortJob;  // Flag to abort during initialization of job

    // Network handling
    QNetworkAccessManager *networkManager;
    QJsonObject pageObj;
    QJsonObject attachmentObj;

    // Settings: Credentials to access Confluence
    bool authUsingPAT;
    QString personalAccessToken;
    QString username;
    QString password;

    // Settings: Where to find Confluggence
    QString baseURL;
    QString apiURL;

    // Backreferences to take action in calling model
    uint branchID;
    uint modelID;

  private:
    // Parameters
    QString pageURL;
    QString newPageName;
    QString uploadPagePath;
    QString userQuery;

    // Page details received from Confluence
    QString pageID;
    QString spaceKey;

    // Child agent for attachments
    ConfluenceAgent *attachmentsAgent;

    // Attachments found in page
    QStringList attachmentsTitles;
    QStringList attachmentsIds;

    // Current attachments queued for upload
    QStringList uploadAttachmentPaths;
    int currentUploadAttachmentIndex;
    QString currentAttachmentPath;      // set with basename(..) from path
    QString currentAttachmentTitle;      // set with basename(..) from path
    QString currentAttachmentId;         // copied from attachmentsIds

    // User info received from Confluence
    QList <ConfluenceUser> userList;
};
#endif
