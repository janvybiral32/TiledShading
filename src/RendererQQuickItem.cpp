#include <QtQuick/qquickwindow.h>
#include <RendererQQuickItem.h>
#include <QThread>
#include <SceneLoader.h>
#include <Renderer.h>
#include <geSG/Scene.h>

ts::RendererQQuickItem::RendererQQuickItem()
{
	m_renderer = std::make_unique<Renderer>();
	m_sceneLoaderThread = std::make_unique<QThread>();
	m_sceneLoader = std::make_unique<SceneLoader>();

	connect(this, &QQuickItem::windowChanged, this, &RendererQQuickItem::onWindowChanged);
	connect(this, &RendererQQuickItem::mouseLeftPressed, m_renderer.get(), &Renderer::onMouseLeftPressed, Qt::DirectConnection);
	connect(this, &RendererQQuickItem::mouseRightPressed, m_renderer.get(), &Renderer::onMouseRightPressed, Qt::DirectConnection);
	connect(this, &RendererQQuickItem::mouseLeftReleased, m_renderer.get(), &Renderer::onMouseLeftReleased, Qt::DirectConnection);
	connect(this, &RendererQQuickItem::mouseRightReleased, m_renderer.get(), &Renderer::onMouseRightReleased, Qt::DirectConnection);
	connect(this, &RendererQQuickItem::mousePositionChanged, m_renderer.get(), &Renderer::onMousePositionChanged, Qt::DirectConnection);
	connect(this, &RendererQQuickItem::keyPressed, m_renderer.get(), &Renderer::onKeyPressed, Qt::DirectConnection);
	connect(m_renderer.get(), &Renderer::renderingFinished, this, &RendererQQuickItem::onRenderingFinished, Qt::DirectConnection);

	qRegisterMetaType<std::shared_ptr<ge::sg::Scene>>("std::shared_ptr<ge::sg::Scene>");
	m_sceneLoader.get()->moveToThread(m_sceneLoaderThread.get());
	connect(this, &RendererQQuickItem::loadScene, this, &RendererQQuickItem::onLoadScene);
	connect(this, &RendererQQuickItem::loadScene, m_sceneLoader.get(), &SceneLoader::loadScene);
	connect(m_sceneLoader.get(), &SceneLoader::sceneLoaded, this, &RendererQQuickItem::onSceneLoaded);
	connect(m_sceneLoader.get(), &SceneLoader::sceneLoadingFailed, this, &RendererQQuickItem::onSceneLoadingFailed);
	m_sceneLoaderThread->start();
}

ts::RendererQQuickItem::~RendererQQuickItem()
{
	m_sceneLoaderThread->quit();
	m_sceneLoaderThread->wait();
}

void ts::RendererQQuickItem::setFovy(int value)
{
	if (value != getFovy())
	{
		m_renderer->setFovy(value);

		emit fovyChanged(value);
	}
}

void ts::RendererQQuickItem::setMovementSpeed(int value)
{
	if (value != getMovementSpeed())
	{
		m_renderer->setMovementSpeed(value / 1000.0f);

		emit movementSpeedChanged(value);
	}
}

void ts::RendererQQuickItem::setLightCount(int value)
{
	bool changed = false;
	if (value != getLightCount())
	{
		changed = true;
	}

	m_renderer->generateLights(value);

	if (changed)
	{
		emit lightCountChanged(value);
	}
}

void ts::RendererQQuickItem::setLightPosRange(int value)
{
	if (value != getLightPosRange())
	{
		m_renderer->setLightPosRange(value / 1000.0f);

		emit lightPosRangeChanged(value);
	}
}

void ts::RendererQQuickItem::setPointLightRadiusMin(int value)
{
	if (value != getPointLightRadiusMin())
	{
		m_renderer->setPointLightRadiusMin(value / 1000.0f);

		emit pointLightRadiusMinChanged(value);
	}
}

void ts::RendererQQuickItem::setPointLightRadiusMax(int value)
{
	if (value != getPointLightRadiusMax())
	{
		m_renderer->setPointLightRadiusMax(value / 1000.0f);

		emit pointLightRadiusMaxChanged(value);
	}
}

void ts::RendererQQuickItem::setCameraType(CameraType type)
{
	if (type != getCameraType())
	{
		m_renderer->setCameraType(static_cast<Renderer::CameraType>(type));

		emit cameraTypeChanged(type);
	}
}

int ts::RendererQQuickItem::getFovy() const
{
	return static_cast<int>(m_renderer->getFovy());
}

int ts::RendererQQuickItem::getLightCount() const
{
	return m_renderer->getLightCount();
}

int ts::RendererQQuickItem::getLightPosRange() const
{
	return static_cast<int>(m_renderer->getLightPosRange() * 1000);
}

int ts::RendererQQuickItem::getPointLightRadiusMin() const
{
	return static_cast<int>(m_renderer->getPointLightRadiusMin() * 1000);
}

int ts::RendererQQuickItem::getPointLightRadiusMax() const
{
	return static_cast<int>(m_renderer->getPointLightRadiusMax() * 1000);
}

int ts::RendererQQuickItem::getMovementSpeed() const
{
	return static_cast<int>(m_renderer->getMovementSpeed() * 1000);
}

ts::RendererQQuickItem::CameraType ts::RendererQQuickItem::getCameraType() const
{
	return static_cast<CameraType>(m_renderer->getCameraType());
}

void ts::RendererQQuickItem::onWindowChanged(QQuickWindow* win)
{
	if (win != nullptr)
	{
		connect(win, &QQuickWindow::beforeSynchronizing, this, &RendererQQuickItem::onSynchronize, Qt::DirectConnection);
		connect(win, &QQuickWindow::sceneGraphInvalidated, this, &RendererQQuickItem::onSceneGraphInvalidated, Qt::DirectConnection);
		connect(win, &QQuickWindow::beforeRendering, m_renderer.get(), &Renderer::onRender, Qt::DirectConnection);
		connect(m_renderer.get(), &Renderer::redraw, win, &QQuickWindow::update, Qt::DirectConnection);

		// If we allow QML to do the clearing, they would clear what we paint
		// and nothing would show.
		win->setClearBeforeRendering(false);
	}
}

void ts::RendererQQuickItem::onSceneGraphInvalidated()
{
	//qDebug() << "Context will be released";
}

void ts::RendererQQuickItem::onRenderingFinished(float time)
{
	if (m_renderTime != time)
	{
		m_renderTime = time;
		emit renderTimeChanged(time);
	}
}

void ts::RendererQQuickItem::onSceneLoaded(std::shared_ptr<ge::sg::Scene> scene)
{
	m_renderer->setScene(scene);

	m_sceneLoading = false;
	emit sceneLoadingChanged(false);
}

void ts::RendererQQuickItem::onSceneLoadingFailed()
{
	qDebug() << "scene load failed";

	m_sceneLoading = false;
	emit sceneLoadingChanged(false);
}

void ts::RendererQQuickItem::onLoadScene(const QUrl& sceneFile)
{
	m_sceneLoading = true;
	emit sceneLoadingChanged(true);
}

void ts::RendererQQuickItem::onSynchronize()
{
	m_renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
	m_renderer->setWindow(window());
}
