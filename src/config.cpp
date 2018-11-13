#include "main.h"
#include "container.h"
#include "backend.h"
#include "config.h"
#include <xcb/xcb_keysyms.h> //todo: should not depend on xcb here
#include <X11/keysym.h>

namespace Config{

ContainerInterface::ContainerInterface() :
borderWidth(boost::python::make_tuple(0.0f,0.0f)),
minSize(boost::python::make_tuple(0.0f,0.0f)),
maxSize(boost::python::make_tuple(1.0f,1.0f)){
	//
}

ContainerInterface::~ContainerInterface(){
	//
}

void ContainerInterface::CopySettingsSetup(WManager::Container::Setup &setup){
	setup.borderWidth.x = boost::python::extract<float>(borderWidth[0])();
	setup.borderWidth.y = boost::python::extract<float>(borderWidth[1])();
	setup.minSize.x = boost::python::extract<float>(minSize[0])();
	setup.minSize.y = boost::python::extract<float>(minSize[1])();
	setup.maxSize.x = boost::python::extract<float>(maxSize[0])();
	setup.maxSize.y = boost::python::extract<float>(maxSize[1])();
}

void ContainerInterface::CopySettingsContainer(){
	pcontainer->borderWidth.x = boost::python::extract<float>(borderWidth[0])();
	pcontainer->borderWidth.y = boost::python::extract<float>(borderWidth[1])();
	pcontainer->minSize.x = boost::python::extract<float>(minSize[0])();
	pcontainer->minSize.y = boost::python::extract<float>(minSize[1])();
	pcontainer->maxSize.x = boost::python::extract<float>(maxSize[0])();
	pcontainer->maxSize.y = boost::python::extract<float>(maxSize[1])();

	for(WManager::Container *pcontainter1 = pcontainer->pch; pcontainter1; pcontainter1 = pcontainter1->pnext){
		ContainerConfig *pcontainerConfig = dynamic_cast<ContainerConfig *>(pcontainter1);
		if(!pcontainerConfig)
			break;
		pcontainerConfig->pcontainerInt->CopySettingsContainer();
	}
}

void ContainerInterface::OnSetup(){
	//
}

boost::python::object ContainerInterface::OnParent(){
	//
	return BackendInterface::GetFocus(); //TODO: sensible default
}

void ContainerInterface::OnCreate(){
	//TODO: focus on this
}

void ContainerInterface::OnPropertyChange(PROPERTY_ID id){
	//
}

boost::python::object ContainerInterface::GetNext() const{
	if(!pcontainer)
		return boost::python::object();
	WManager::Container *pnext = pcontainer->GetNext();
	ContainerConfig *pcontainer1 = dynamic_cast<ContainerConfig *>(pnext);
	if(pcontainer1)
		return pcontainer1->pcontainerInt->self;
	return boost::python::object();
}

boost::python::object ContainerInterface::GetPrev() const{
	if(!pcontainer)
		return boost::python::object();
	//TODO: do this in script?
	/*if(pcontainer->mode == WManager::Container::MODE_FLOATING){
		ContainerConfig *pcontainer1 = dynamic_cast<ContainerConfig *>(pcontainer);
		const WManager::Container *ptreeFocus = pcontainer1->pbackend->GetRoot();
		for(const WManager::Container *pcontainer = ptreeFocus; pcontainer; ptreeFocus = pcontainer, pcontainer = pcontainer->GetFocus());

		pcontainer1 = dynamic_cast<ContainerConfig *>(ptreeFocus);
		return ptreeFocus;
	}*/
	//if floating, ResetFocus(...) //pcontainer1->pbackend->GetRoot()
	WManager::Container *pPrev = pcontainer->GetPrev();
	ContainerConfig *pcontainer1 = dynamic_cast<ContainerConfig *>(pPrev);
	if(pcontainer1)
		return pcontainer1->pcontainerInt->self;
	return boost::python::object();
}

boost::python::object ContainerInterface::GetParent() const{
	if(!pcontainer)
		return boost::python::object();
	WManager::Container *pParent = pcontainer->GetParent();
	if(!pParent)
		return boost::python::object();
	ContainerConfig *pcontainer1 = dynamic_cast<ContainerConfig *>(pParent);
	if(pcontainer1)
		return pcontainer1->pcontainerInt->self;
	return boost::python::object();
}

boost::python::object ContainerInterface::GetFocus() const{
	if(!pcontainer)
		return boost::python::object();
	WManager::Container *pfocus = pcontainer->GetFocus();
	ContainerConfig *pcontainer1 = dynamic_cast<ContainerConfig *>(pfocus);
	if(pcontainer1)
		return pcontainer1->pcontainerInt->self;
	return boost::python::object();
}

boost::python::object ContainerInterface::GetFloatFocus() const{
	if(!pcontainer)
		return boost::python::object();
	auto m = std::find(BackendInterface::floatFocusQueue.begin(),BackendInterface::floatFocusQueue.end(),pcontainer);
	if(m == BackendInterface::floatFocusQueue.begin() || m == BackendInterface::floatFocusQueue.end()){
		if(BackendInterface::floatFocusQueue.size() == 0)
			return boost::python::object();
		ContainerConfig *pcontainer1 = dynamic_cast<ContainerConfig *>(BackendInterface::floatFocusQueue.back());
		if(pcontainer1)
			return pcontainer1->pcontainerInt->self;
		return boost::python::object();
	}

	ContainerConfig *pcontainer1 = dynamic_cast<ContainerConfig *>(*(m-1));
	if(pcontainer1)
		return pcontainer1->pcontainerInt->self;
	return boost::python::object();
}

boost::python::object ContainerInterface::GetAdjacent(WManager::Container::ADJACENT a) const{
	if(!pcontainer)
		return boost::python::object();
	WManager::Container *padj = pcontainer->GetAdjacent(a);
	ContainerConfig *pcontainer1 = dynamic_cast<ContainerConfig *>(padj);
	if(pcontainer1)
		return pcontainer1->pcontainerInt->self;
	return boost::python::object();
}

void ContainerInterface::Move(boost::python::object containerObject){
	//
	boost::python::extract<Config::ContainerInterface &> containerExtract(containerObject);
	if(!containerExtract.check()){
		DebugPrintf(stderr,"Move(): Invalid container parameter.\n");
		return;
	}

	Config::ContainerInterface &containerInt = containerExtract();
	if(!containerInt.pcontainer)
		return;
	ContainerConfig *pcontainer1 = dynamic_cast<ContainerConfig *>(containerInt.pcontainer);
	if(pcontainer1)
		pcontainer1->pbackend->MoveContainer(pcontainer,containerInt.pcontainer);
}

ContainerProxy::ContainerProxy() : ContainerInterface(){
	//
}

ContainerProxy::~ContainerProxy(){
	//
}

void ContainerProxy::OnSetup(){
	boost::python::override ovr = this->get_override("OnSetup");
	if(ovr){
		try{
			ovr();
		}catch(boost::python::error_already_set &){
			PyErr_Print();
			//
			boost::python::handle_exception();
			PyErr_Clear();
		}
	}else ContainerInterface::OnSetup();
}

boost::python::object ContainerProxy::OnParent(){
	boost::python::override ovr = this->get_override("OnParent");
	if(ovr){
		try{
			return ovr();
		}catch(boost::python::error_already_set &){
			PyErr_Print();
			//
			boost::python::handle_exception();
			PyErr_Clear();
		}
	}
	return ContainerInterface::OnParent();
}

void ContainerProxy::OnCreate(){
	boost::python::override ovr = this->get_override("OnCreate");
	if(ovr){
		try{
			ovr();
		}catch(boost::python::error_already_set &){
			PyErr_Print();
			//
			boost::python::handle_exception();
			PyErr_Clear();
		}
	}else ContainerInterface::OnCreate();
}

void ContainerProxy::OnPropertyChange(PROPERTY_ID id){
	boost::python::override ovr = this->get_override("OnPropertyChange");
	if(ovr){
		try{
			ovr(id);
		}catch(boost::python::error_already_set &){
			PyErr_Print();
			//
			boost::python::handle_exception();
			PyErr_Clear();
		}
	}else return ContainerInterface::OnPropertyChange(id);
}

ContainerConfig::ContainerConfig(ContainerInterface *_pcontainerInt, Backend::X11Backend *_pbackend) : pcontainerInt(_pcontainerInt), pbackend(_pbackend){
	//
}

ContainerConfig::ContainerConfig(Backend::X11Backend *_pbackend) : pbackend(_pbackend){
	//TODO: setting root node properties (e.g. maxSize) should be allowed, this can be used for example
	//to make space for docks and other widgets
	try{
		//https://code.activestate.com/lists/python-cplusplus-sig/17025/
		const char *prootsrc =
			"import chamfer\n"
			"class RootContainer(chamfer.Container):\n"
			"    pass;\n";
		boost::python::object main = boost::python::import("__main__");
		boost::python::object global(main.attr("__dict__"));

		boost::python::object result = boost::python::exec(prootsrc,global,global);
		boost::python::object type = global["RootContainer"];

		boost::python::object object = type();
		boost::python::incref(object.ptr());
		pcontainerInt = &boost::python::extract<Config::ContainerInterface &>(object)();
		pcontainerInt->self = object;

	}catch(boost::python::error_already_set &){

		PyErr_Print();
	}
}

ContainerConfig::~ContainerConfig(){
	//
}

/*template<typename T>
BackendContainerConfig<T>::BackendContainerConfig(ContainerInterface *_pcontainerInt, WManager::Container *_pParent, const WManager::Container::Setup &_setup, Backend::X11Backend *_pbackend) : T(_pParent,_setup,_pbackend), ContainerConfig(_pcontainerInt){
	//
}

template<typename T>
BackendContainerConfig<T>::BackendContainerConfig(Backend::X11Backend *_pbackend) : T(_pbackend), ContainerConfig(){
	//
	pcontainerInt->pcontainer = this;
}

template<typename T>
BackendContainerConfig<T>::~BackendContainerConfig(){
	//
}

class template BackendContainerConfig<Backend::X11Container>;*/

X11ContainerConfig::X11ContainerConfig(ContainerInterface *_pcontainerInt, WManager::Container *_pParent, const WManager::Container::Setup &_setup, Backend::X11Backend *_pbackend) : Backend::X11Container(_pParent,_setup,_pbackend), ContainerConfig(_pcontainerInt,_pbackend){
	//
}

X11ContainerConfig::X11ContainerConfig(Backend::X11Backend *_pbackend) : Backend::X11Container(_pbackend), ContainerConfig(_pbackend){
	//
	pcontainerInt->pcontainer = this;
}

X11ContainerConfig::~X11ContainerConfig(){
	//
}

DebugContainerConfig::DebugContainerConfig(ContainerInterface *_pcontainerInt, WManager::Container *_pParent, const WManager::Container::Setup &_setup, Backend::X11Backend *_pbackend) : Backend::DebugContainer(_pParent,_setup,_pbackend), ContainerConfig(_pcontainerInt,_pbackend){
	//
}

DebugContainerConfig::DebugContainerConfig(Backend::X11Backend *_pbackend) : Backend::DebugContainer(_pbackend), ContainerConfig(_pbackend){
	//
	pcontainerInt->pcontainer = this;
}

DebugContainerConfig::~DebugContainerConfig(){
	//
}

BackendInterface::BackendInterface(){
	//
}

BackendInterface::~BackendInterface(){
	//
}

void BackendInterface::OnSetupKeys(Backend::X11KeyBinder *pkeyBinder, bool debug){
	//
	DebugPrintf(stdout,"No KeyConfig interface, skipping configuration.\n");
}

boost::python::object BackendInterface::OnCreateContainer(){
	//TODO: should create a default instance here
	return boost::python::object();
}

void BackendInterface::OnKeyPress(uint keyId){
	//
}

void BackendInterface::OnKeyRelease(uint keyId){
	//
}

void BackendInterface::OnTimer(){
	//
}

void BackendInterface::Bind(boost::python::object obj){
	BackendInterface &backendInt1 = boost::python::extract<BackendInterface&>(obj)();
	pbackendInt = &backendInt1;
}

void BackendInterface::SetFocus(WManager::Container *pcontainer){
	if(!pcontainer)
		return;
	if(pcontainer->mode == WManager::Container::MODE_FLOATING){
		floatFocusQueue.erase(std::remove(floatFocusQueue.begin(),floatFocusQueue.end(),pcontainer));
		floatFocusQueue.push_back(pcontainer);
	}
	pfocus = pcontainer;
	pcontainer->Focus();
}

boost::python::object BackendInterface::GetFocus(){
	ContainerConfig *pcontainer1 = dynamic_cast<ContainerConfig *>(pfocus);
	if(pcontainer1)
		return pcontainer1->pcontainerInt->self;
	return boost::python::object();
}

/*void BackendInterface::ResetFocus(){
	//
	WManager::Container *pParent = pfocus;
	for(WManager::Container *pcontainer = pfocus->pParent; pcontainer; pParent = pcontainer, pcontainer = pcontainer->pParent);

	WManager::Container *pNewFocus = pParent;
	for(WManager::Container *pcontainer = pNewFocus; pcontainer; pNewFocus = pcontainer, pcontainer = pcontainer->focusQueue.size() > 0?pcontainer->focusQueue.back():pcontainer->pch);
	SetFocus(pNewFocus);
}*/

boost::python::object BackendInterface::GetRoot(){
	WManager::Container *pParent = pfocus;
	for(WManager::Container *pcontainer = pfocus->pParent; pcontainer; pParent = pcontainer, pcontainer = pcontainer->pParent);
	ContainerConfig *pcontainer1 = dynamic_cast<ContainerConfig *>(pParent);
	if(pcontainer1)
		return pcontainer1->pcontainerInt->self;
	return boost::python::object();
}

BackendInterface BackendInterface::defaultInt;
BackendInterface *BackendInterface::pbackendInt = &BackendInterface::defaultInt;
WManager::Container *BackendInterface::pfocus = 0; //initially set to root container as soon as it's created
std::deque<WManager::Container *> BackendInterface::floatFocusQueue;

BackendProxy::BackendProxy(){
	//
}

BackendProxy::~BackendProxy(){
	//
}

void BackendProxy::OnSetupKeys(Backend::X11KeyBinder *pkeyBinder, bool debug){
	boost::python::override ovr = this->get_override("OnSetupKeys");
	if(ovr){
		try{
			ovr(pkeyBinder,debug);
		}catch(boost::python::error_already_set &){
			PyErr_Print();
			//
			boost::python::handle_exception();
			PyErr_Clear();
		}
	}else BackendInterface::OnSetupKeys(pkeyBinder,debug);
}

boost::python::object BackendProxy::OnCreateContainer(){
	boost::python::override ovr = this->get_override("OnCreateContainer");
	if(ovr){
		try{
			return ovr();
		}catch(boost::python::error_already_set &){
			PyErr_Print();
			//
			boost::python::handle_exception();
			PyErr_Clear();
		}
	}
	return BackendInterface::OnCreateContainer();
}

void BackendProxy::OnKeyPress(uint keyId){
	boost::python::override ovr = this->get_override("OnKeyPress");
	if(ovr){
		try{
			ovr(keyId);
		}catch(boost::python::error_already_set &){
			PyErr_Print();
			//
			boost::python::handle_exception();
			PyErr_Clear();
		}
	}else BackendInterface::OnKeyPress(keyId);
}

void BackendProxy::OnKeyRelease(uint keyId){
	boost::python::override ovr = this->get_override("OnKeyRelease");
	if(ovr){
		try{
			ovr(keyId);
		}catch(boost::python::error_already_set &){
			PyErr_Print();
			//
			boost::python::handle_exception();
			PyErr_Clear();
		}
	}else BackendInterface::OnKeyRelease(keyId);
}

void BackendProxy::OnTimer(){
	boost::python::override ovr = this->get_override("OnTimer");
	if(ovr){
		try{
			ovr();
		}catch(boost::python::error_already_set &){
			PyErr_Print();
			//
			boost::python::handle_exception();
			PyErr_Clear();
		}
	}else BackendInterface::OnTimer();
}

CompositorInterface::CompositorInterface() : shaderPath("."){
	//
}

CompositorInterface::~CompositorInterface(){
	//
}

void CompositorInterface::Bind(boost::python::object obj){
	CompositorInterface &compositorInt = boost::python::extract<CompositorInterface&>(obj)();
	pcompositorInt = &compositorInt;
}

CompositorInterface CompositorInterface::defaultInt;
CompositorInterface *CompositorInterface::pcompositorInt = &CompositorInterface::defaultInt;

CompositorProxy::CompositorProxy(){
	//
}

CompositorProxy::~CompositorProxy(){
	//
}

BOOST_PYTHON_MODULE(chamfer){
	boost::python::scope().attr("MOD_MASK_1") = uint(XCB_MOD_MASK_1);
	boost::python::scope().attr("MOD_MASK_2") = uint(XCB_MOD_MASK_2);
	boost::python::scope().attr("MOD_MASK_3") = uint(XCB_MOD_MASK_3);
	boost::python::scope().attr("MOD_MASK_4") = uint(XCB_MOD_MASK_4);
	boost::python::scope().attr("MOD_MASK_5") = uint(XCB_MOD_MASK_5);
	boost::python::scope().attr("MOD_MASK_SHIFT") = uint(XCB_MOD_MASK_SHIFT);
	boost::python::scope().attr("MOD_MASK_CONTROL") = uint(XCB_MOD_MASK_CONTROL);

	boost::python::scope().attr("KEY_RETURN") = uint(XK_Return);
	boost::python::scope().attr("KEY_SPACE") = uint(XK_space);
	boost::python::scope().attr("KEY_TAB") = uint(XK_Tab);

	boost::python::class_<Backend::X11KeyBinder>("KeyBinder",boost::python::no_init)
		.def("BindKey",&Backend::X11KeyBinder::BindKey)
		;
	
	boost::python::enum_<ContainerInterface::PROPERTY_ID>("property")
		.value("NAME",ContainerInterface::PROPERTY_ID_NAME)
		.value("CLASS",ContainerInterface::PROPERTY_ID_CLASS);

	boost::python::class_<ContainerProxy,boost::noncopyable>("Container")
		.def("OnSetup",&ContainerInterface::OnSetup)
		.def("OnParent",&ContainerInterface::OnParent)
		.def("OnCreate",&ContainerInterface::OnCreate)
		.def("OnProperty",&ContainerInterface::OnPropertyChange)
		.def("GetNext",&ContainerInterface::GetNext)
		.def("GetPrev",&ContainerInterface::GetPrev)
		.def("GetParent",&ContainerInterface::GetParent)
		.def("GetFocus",&ContainerInterface::GetFocus)
		.def("GetFloatFocus",&ContainerInterface::GetFloatFocus)
		.def("GetAdjacent",&ContainerInterface::GetAdjacent)
		.def("MoveNext",boost::python::make_function(
			[](ContainerInterface &container){
				container.pcontainer->MoveNext();
			},boost::python::default_call_policies(),boost::mpl::vector<void, ContainerInterface &>()))
		.def("MovePrev",boost::python::make_function(
			[](ContainerInterface &container){
				container.pcontainer->MovePrev();
			},boost::python::default_call_policies(),boost::mpl::vector<void, ContainerInterface &>()))
		.def("Move",&ContainerInterface::Move)
		.def("Focus",boost::python::make_function(
			[](ContainerInterface &container){
				Config::BackendInterface::pfocus = container.pcontainer;
				container.pcontainer->Focus();
			},boost::python::default_call_policies(),boost::mpl::vector<void, ContainerInterface &>()))
		.def("Kill",boost::python::make_function(
			[](ContainerInterface &container){
				if(container.pcontainer->pclient)
					container.pcontainer->pclient->Kill();
			},boost::python::default_call_policies(),boost::mpl::vector<void, ContainerInterface &>()))
		.def("ShiftLayout",boost::python::make_function(
			[](ContainerInterface &container, WManager::Container::LAYOUT layout){
				container.CopySettingsContainer();
				container.pcontainer->SetLayout(layout);
			},boost::python::default_call_policies(),boost::mpl::vector<void, ContainerInterface &, WManager::Container::LAYOUT>()))
		.def_readwrite("borderWidth",&ContainerInterface::borderWidth)
		.def_readwrite("minSize",&ContainerInterface::minSize)
		.def_readwrite("maxSize",&ContainerInterface::maxSize)
		.def_readonly("wm_name",&ContainerInterface::wm_name)
		.def_readonly("wm_class",&ContainerInterface::wm_class)
		.add_property("layout",boost::python::make_function(
			[](ContainerInterface &container){
				return container.pcontainer->layout;
			},boost::python::default_call_policies(),boost::mpl::vector<WManager::Container::LAYOUT, ContainerInterface &>()))
		.add_property("mode",boost::python::make_function(
			[](ContainerInterface &container){
				return container.pcontainer->mode;
			},boost::python::default_call_policies(),boost::mpl::vector<WManager::Container::MODE, ContainerInterface &>()))
		;
	
	boost::python::enum_<WManager::Container::ADJACENT>("adjacent")
		.value("LEFT",WManager::Container::ADJACENT_LEFT)
		.value("RIGHT",WManager::Container::ADJACENT_RIGHT)
		.value("UP",WManager::Container::ADJACENT_UP)
		.value("DOWN",WManager::Container::ADJACENT_DOWN);

	boost::python::enum_<WManager::Container::LAYOUT>("layout")
		.value("VSPLIT",WManager::Container::LAYOUT_VSPLIT)
		.value("HSPLIT",WManager::Container::LAYOUT_HSPLIT);

	boost::python::enum_<WManager::Container::MODE>("mode")
		.value("TILED",WManager::Container::MODE_TILED)
		.value("FLOATING",WManager::Container::MODE_FLOATING);

	boost::python::class_<BackendProxy,boost::noncopyable>("Backend")
		.def("OnSetupKeys",&BackendInterface::OnSetupKeys)
		.def("OnCreateContainer",&BackendInterface::OnCreateContainer)
		.def("OnKeyPress",&BackendInterface::OnKeyPress)
		.def("OnKeyRelease",&BackendInterface::OnKeyRelease)
		.def("OnTimer",&BackendInterface::OnTimer)
		;
	boost::python::def("bind_Backend",BackendInterface::Bind);
	boost::python::class_<CompositorProxy,boost::noncopyable>("Compositor")
		.add_property("shaderPath",&CompositorInterface::shaderPath)
		;
	boost::python::def("bind_Compositor",CompositorInterface::Bind);

	boost::python::def("GetFocus",&BackendInterface::GetFocus);
	boost::python::def("GetRoot",&BackendInterface::GetRoot);
}

Loader::Loader(const char *pargv0){
	wchar_t wargv0[1024];
	mbtowc(wargv0,pargv0,sizeof(wargv0)/sizeof(wargv0[0]));
	Py_SetProgramName(wargv0);

	PyImport_AppendInittab("chamfer",PyInit_chamfer);
	Py_Initialize();
}

Loader::~Loader(){
	Py_Finalize();
}

void Loader::Run(const char *pfilePath, const char *pfileLabel){
	FILE *pf = fopen(pfilePath,"rb");
	if(!pf){
		DebugPrintf(stderr,"Unable to open configuration file %s\n",pfilePath);
		return;
	}
	try{
		PyRun_SimpleFile(pf,pfileLabel);

	}catch(boost::python::error_already_set &){
		boost::python::handle_exception();
		PyErr_Clear();
	}
	fclose(pf);
}

}

