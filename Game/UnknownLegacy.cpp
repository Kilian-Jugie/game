#include "UnknownLegacy.h"
namespace  UL {
	UnknownLegacy::UnknownLegacy(const Arguments& arguments) :
		Platform::Application{ arguments, Configuration{}
			.setTitle("Putain c'est trop simple")
			.setWindowFlags(Configuration::WindowFlag::Resizable) }
	{
		testDebug();

		std::exit(0);

		Utility::Arguments args;
		args.addArgument("file").setHelp("file", "file to load")
			.addOption("importer", "AnySceneImporter").setHelp("importer", "importer plugin to use")
			.addSkippedPrefix("magnum").setHelp("engine-specific options")
			.setHelp("Displays a 3D scene file provided on command line.")
			.parse(arguments.argc, arguments.argv);

		

		/* Load a scene importer plugin */
		PluginManager::Manager<Trade::AbstractImporter> manager;
		std::unique_ptr<Trade::AbstractImporter> importer = manager.loadAndInstantiate(args.value("importer"));
		if (!importer) std::exit(1);

		Debug{} << "Opening file" << args.value("file");

		m_Renderer.loadFile(importer.get(), args.value("file"));
	}
}

MAGNUM_APPLICATION_MAIN(UL::UnknownLegacy)

