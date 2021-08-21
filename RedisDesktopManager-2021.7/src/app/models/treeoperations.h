#pragma once
#include <QEnableSharedFromThis>
#include <QObject>
#include <QSharedPointer>
#include <functional>
#include "app/models/connectionconf.h"
#include "modules/bulk-operations/bulkoperationsmanager.h"
#include "modules/connections-tree/operations.h"
#include "connections-tree/items/keyitem.h"

class Events;

namespace ConnectionsTree {
    class ServerItem;
}

class TreeOperations : public QObject,
                       public ConnectionsTree::Operations,
                       public QEnableSharedFromThis<TreeOperations> {
  Q_OBJECT
 public:
  TreeOperations(const ServerConfig& config,
                 QSharedPointer<Events> events);

  ~TreeOperations();

  QFuture<void> getDatabases(std::function<void (RedisClient::DatabaseList, const QString&)>) override;

  void loadNamespaceItems(
      QSharedPointer<ConnectionsTree::AbstractNamespaceItem> parent,
      const QString& filter, std::function<void(const QString& err)> callback,
      QSet<QByteArray> expandedNs) override;

  void disconnect() override;

  void resetConnection() override;

  QString getNamespaceSeparator() override;

  QString defaultFilter() override;

  QVariantMap getFilterHistory() override;

  QString connectionName() const override;

  void openKeyTab(QSharedPointer<ConnectionsTree::KeyItem> key,
                  bool openInNewTab = false) override;

  void openConsoleTab(int dbIndex = 0) override;

  void openNewKeyDialog(int dbIndex, std::function<void()> callback,
                        QString keyPrefix = QString()) override;

  void openServerStats() override;

  void duplicateConnection() override;

  void notifyDbWasUnloaded(int dbIndex) override;

  void deleteDbKey(ConnectionsTree::KeyItem& key,
                   std::function<void(const QString&)> callback) override;

  virtual void deleteDbKeys(ConnectionsTree::DatabaseItem& db) override;

  void deleteDbNamespace(ConnectionsTree::NamespaceItem& ns) override;

  virtual void setTTL(ConnectionsTree::AbstractNamespaceItem& ns) override;

  virtual void copyKeys(ConnectionsTree::AbstractNamespaceItem& ns) override;

  virtual void importKeysFromRdb(ConnectionsTree::DatabaseItem& ns) override;

  virtual void flushDb(int dbIndex,
                       std::function<void(const QString&)> callback) override;

  virtual QFuture<bool> connectionSupportsMemoryOperations() override;

  virtual void openKeyIfExists(const QByteArray& key,
                               QSharedPointer<ConnectionsTree::DatabaseItem> parent,
                               std::function<void(const QString&, bool)> callback) override;

  virtual void getUsedMemory(const QList<QByteArray>& keys, int dbIndex,
      std::function<void(qlonglong)> result,
      std::function<void(qlonglong)> progress) override;

  virtual QString mode() override;

  virtual bool isConnected() const override;

  QSharedPointer<RedisClient::Connection> connection();

  void setConnection(QSharedPointer<RedisClient::Connection> c);

  ServerConfig config();

  void setConfig(const ServerConfig& c);

signals:
  void createNewConnection(const ServerConfig& config);

  void configUpdated();

  void filterHistoryUpdated();

 protected:
  void loadDatabases(QSharedPointer<AsyncFuture::Deferred<void>> d,
                     std::function<void(RedisClient::DatabaseList, const QString&)> callback);

  void recursiveSelectScan(QSharedPointer<AsyncFuture::Deferred<void>> d,
                           QSharedPointer<RedisClient::Connection> c,
                           QSharedPointer<RedisClient::DatabaseList> dbList,
                           std::function<void(RedisClient::DatabaseList, const QString&)> callback);

  bool connect(QSharedPointer<RedisClient::Connection> c);

  void requestBulkOperation(
      ConnectionsTree::AbstractNamespaceItem& ns,
      BulkOperations::Manager::Operation op,
      BulkOperations::AbstractOperation::OperationCallback callback);

 private:
  QSharedPointer<RedisClient::Connection> m_connection;
  QSharedPointer<Events> m_events;
  uint m_dbCount;
  RedisClient::Connection::Mode m_connectionMode;
  ServerConfig m_config;
  QVariantMap m_filterHistory;
  QWeakPointer<ConnectionsTree::ServerItem> m_serverItem;
  QSharedPointer<AsyncFuture::Deferred<void>> m_dbScanOp;
};
