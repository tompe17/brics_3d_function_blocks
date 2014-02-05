/*
 * A demo function block
 */

#define DEBUG 1
#define BRICS_MICROBLX_ENABLE

/* std includes */
#include <iostream>
#include <cstring>

/* microblx includes */
#include "ubx.h"

/* BRICS_3D includes */
#include <brics_3d/core/Logger.h>
#include <brics_3d/worldModel/WorldModel.h>
#include <brics_3d/worldModel/sceneGraph/DotGraphGenerator.h>

using namespace std;
using brics_3d::Logger;

brics_3d::WorldModel* wmHandle = 0;
brics_3d::rsg::DotGraphGenerator* wmPrinter = 0;

/* function block meta-data
 * used by higher level functions.
 */
char testblock_meta[] =
	"{ doc='A testblock number generator function block',"
	"  license='LGPL',"
	"  real-time=true,"
	"}";

/* configuration
 * upon cloning the following happens:
 *   - value.type is resolved
 *   - value.data will point to a buffer of size value.len*value.type->size
 *
 * if an array is required, then .value = { .len=<LENGTH> } can be used.
 */
ubx_config_t testblock_config[] = {
	{ .name="test_conf", .type_name = "double" },
	{ NULL },
};


ubx_port_t testblock_ports[] = {
	{ .name="foo", .attrs=PORT_DIR_IN, .in_type_name="unsigned int" },
	{ .name="bar", .attrs=PORT_DIR_OUT, .out_type_name="unsigned int" },
	{ NULL },
};

static int testblock_init(ubx_block_t *c)
{
	LOG(INFO) << "testblock_init: hi from " << c->name;

	wmHandle = brics_3d::WorldModel::microBlxWmHandle;
	wmPrinter = new brics_3d::rsg::DotGraphGenerator();

	return 0;
}


static void testblock_cleanup(ubx_block_t *c)
{
	if (wmPrinter != 0) {
		delete wmPrinter;
		wmPrinter = 0;
	}

	LOG(INFO) << "testblock_cleanup: hi from " << c->name;
}

static int testblock_start(ubx_block_t *c)
{
	LOG(INFO) << "testblock_start: hi from " << c->name;
	return 0; /* Ok */
}

static void testblock_step(ubx_block_t *c) {
	LOG(INFO) << "testblock_step: hi from " << c->name;

	/* Just print what the world model has to offer. */
	wmPrinter->reset();
	wmHandle->scene.executeGraphTraverser(wmPrinter, wmHandle->getRootNodeId());
	LOG(INFO) << "Current state of the world model: " << std::endl << wmPrinter->getDotGraph();
}


/* put everything together */
ubx_block_t testblock_comp = {
	.name = "testblock/testblock",
	.type = BLOCK_TYPE_COMPUTATION,
	.meta_data = testblock_meta,
	.configs = testblock_config,
	.ports = testblock_ports,

	/* ops */
	.init = testblock_init,
	.start = testblock_start,
	.step = testblock_step,
	.cleanup = testblock_cleanup,
};

static int testblock_init(ubx_node_info_t* ni)
{
	LOG(DEBUG) << "testblock_init(ubx_node_info_t* ni)";
	return ubx_block_register(ni, &testblock_comp);
}

static void testblock_cleanup(ubx_node_info_t *ni)
{
	LOG(DEBUG) << "testblock_cleanup(ubx_node_info_t *ni)";
	ubx_block_unregister(ni, "testblock/testblock");
}

UBX_MODULE_INIT(testblock_init)
UBX_MODULE_CLEANUP(testblock_cleanup)