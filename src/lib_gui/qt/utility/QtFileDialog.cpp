#include "qt/utility/QtFileDialog.h"

#include <QFileDialog>
#include <QListView>
#include <QTreeView>

#include "qt/utility/QtFilesAndDirectoriesDialog.h"
#include "utility/file/FilePath.h"
#include "utility/utilityApp.h"

QStringList QtFileDialog::getFileNamesAndDirectories(QWidget* parent, const FilePath& path)
{
	const QString dir = getDir(QString::fromStdString((path.isDirectory() ? path : path.getParentDirectory()).str()));

	QFileDialog* dialog = (utility::getOsType() == OS_MAC ? new QFileDialog(parent) : new QtFilesAndDirectoriesDialog(parent));

	if (!dir.isEmpty())
	{
		dialog->setDirectory(dir);
	}

	QListView *l = dialog->findChild<QListView*>("listView");
	if (l)
	{
		l->setSelectionMode(QAbstractItemView::ExtendedSelection);
	}
	QTreeView *t = dialog->findChild<QTreeView*>();
	if (t)
	{
		t->setSelectionMode(QAbstractItemView::ExtendedSelection);
	}

	QStringList list;
	if (dialog->exec())
	{
		list = dialog->selectedFiles();
	}
	
	delete dialog;

	return list;
}

QString QtFileDialog::getExistingDirectory(QWidget* parent, const QString& caption, const FilePath& dir)
{
	return QFileDialog::getExistingDirectory(parent, caption, getDir(QString::fromStdString(dir.str())));
}

QString QtFileDialog::getOpenFileName(QWidget* parent, const QString& caption, const FilePath& dir, const QString& filter)
{
	return QFileDialog::getOpenFileName(parent, caption, getDir(QString::fromStdString(dir.str())), filter);
}

QString QtFileDialog::showSaveFileDialog(
	QWidget *parent, const QString& title, const FilePath& directory, const QString& filter)
{
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)

	return QFileDialog::getSaveFileName(parent, title, getDir(QString::fromStdString(directory.str())), filter);

#else
	QFileDialog dialog(parent, title, getDir(QString::fromStdString(directory.str())), filter);

	if (parent)
	{
		dialog.setWindowModality(Qt::WindowModal);
	}

	QRegExp filter_regex(QLatin1String("(?:^\\*\\.(?!.*\\()|\\(\\*\\.)(\\w+)"));
	QStringList filters = filter.split(QLatin1String(";;"));

	if (!filters.isEmpty())
	{
		dialog.setNameFilters(filters);
	}

	dialog.setAcceptMode(QFileDialog::AcceptSave);

	if (dialog.exec() == QDialog::Accepted)
	{
		QString file_name = dialog.selectedFiles().constFirst();
		QFileInfo info(file_name);

		if (info.suffix().isEmpty() && !dialog.selectedNameFilter().isEmpty())
		{
			if (filter_regex.indexIn(dialog.selectedNameFilter()) != -1)
			{
				QString extension = filter_regex.cap(1);
				file_name += QLatin1String(".") + extension;
			}
		}
		return file_name;
	}
	else
	{
		return QString();
	}
#endif  // Q_OS_MAC || Q_OS_WIN
}

QString QtFileDialog::getDir(QString dir)
{
	static bool used = false;

	if (!used && dir.isEmpty())
	{
		dir = QDir::homePath();
	}

	used = true;

	return dir;
}
