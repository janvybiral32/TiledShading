
#include <LightsGenerationHandler.h>
#include <Scene.h>
#include <AABB.h>
#include <PointLight.h>
#include <Renderer.h>

ts::LightsGenerationHandler::LightsGenerationHandler(const Scene* scene, Renderer* renderer)
	: m_renderer(renderer)
	, m_scene(scene)
{

}

void ts::LightsGenerationHandler::init(const Scene* scene, Renderer* renderer)
{
	m_renderer = renderer;
	m_scene = scene;
}

void ts::LightsGenerationHandler::generate(int count, float posRange, float radiusMin, float radiusMax)
{
	const AABB& boundingBox = m_scene->getBoundingBox();
	std::vector<ge::sg::PointLight> pointLights;
	glm::vec3 center = boundingBox.getCenter();

	for (int i = 0; i < count; i++)
	{
		ge::sg::PointLight light;

		light.color.r = static_cast<float>(rand()) / RAND_MAX;
		light.color.g = static_cast<float>(rand()) / RAND_MAX;
		light.color.b = static_cast<float>(rand()) / RAND_MAX;
		light.color.a = 1.0f;

		glm::vec3 minPos = boundingBox.min * posRange;
		glm::vec3 maxPos = boundingBox.max * posRange;
		light.position.x = minPos.x + ((static_cast<float>(rand()) / RAND_MAX) * (maxPos.x - minPos.x));
		light.position.y = minPos.y + ((static_cast<float>(rand()) / RAND_MAX) * (maxPos.y - minPos.y));
		light.position.z = minPos.z + ((static_cast<float>(rand()) / RAND_MAX) * (maxPos.z - minPos.z));
		light.position.w = 1.0f;

		float minRadius = radiusMin * boundingBox.getLongestSide();
		float maxRadius = radiusMax * boundingBox.getLongestSide();
		light.radius = minRadius + ((static_cast<float>(rand()) / RAND_MAX) * (maxRadius - minRadius));

		pointLights.push_back(light);
	}

	m_renderer->setLights(pointLights);
	
	emit render();
}
