#if 1

#include <openvibe/ov_all.h>
#include <iostream>
#include <stack>
#include <vector>
#include <map>
#include <string>

#include <gtk/gtk.h>
#include <glade/glade.h>

#define AT_GUI_File "../share/openvibe-interface.glade"

#define AT_AttributeId_BoxCenterPosition                   OpenViBE::CIdentifier(0x207C9054, 0x3C841B63)
#define AT_AttributeId_BoxSize                             OpenViBE::CIdentifier(0x7B814CCA, 0x271DF6DD)
#define AT_AttributeId_LinkSourcePosition                  OpenViBE::CIdentifier(0x358AE8B5, 0x0F8BACD1)
#define AT_AttributeId_LinkTargetPosition                  OpenViBE::CIdentifier(0x6267B5C5, 0x676E3E42)
/*
#define AT_AttributeId_                                    OpenViBE::CIdentifier(0x1FA7A38F, 0x54EDBE0B)
#define AT_AttributeId_                                    OpenViBE::CIdentifier(0x4C90D4AD, 0x7A2554EC)
#define AT_AttributeId_                                    OpenViBE::CIdentifier(0x1B32C44C, 0x1905E0E9)
#define AT_AttributeId_                                    OpenViBE::CIdentifier(0x3F0A3B27, 0x570913D2)
#define AT_AttributeId_                                    OpenViBE::CIdentifier(0x73CF34A9, 0x3D95DA4A)
#define AT_AttributeId_                                    OpenViBE::CIdentifier(0x2D9272EB, 0x2E2B1F52)
#define AT_AttributeId_                                    OpenViBE::CIdentifier(0x58C3F253, 0x3C6968F9)
#define AT_AttributeId_                                    OpenViBE::CIdentifier(0x67FCCF68, 0x2EB30C69)
#define AT_AttributeId_                                    OpenViBE::CIdentifier(0x1ADEF355, 0x4F3669DA)
#define AT_AttributeId_                                    OpenViBE::CIdentifier(0x5D72CF63, 0x6B05F549)
#define AT_AttributeId_                                    OpenViBE::CIdentifier(0x1AB61306, 0x2A30C51C)
*/

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Kernel::Player;
using namespace std;

enum
{
	BoxAlgorithm_StringName,
	BoxAlgorithm_StringShortDescription,
	BoxAlgorithm_StringIdentifier,
	BoxAlgorithm_StringStockIcon,
	BoxAlgorithm_BooleanIsPlugin,
};

enum
{
	Color_BoxBackground,
	Color_BoxBackgroundSelected,
	Color_BoxBorder,
	Color_BoxBorderSelected,
	Color_BoxInputBackground,
	Color_BoxInputBorder,
	Color_BoxOutputBackground,
	Color_BoxOutputBorder,
	Color_BoxSettingBackground,
	Color_BoxSettingBorder,
	Color_Link,
	Color_LinkSelected,
};

enum
{
	Connector_None,
	Connector_Input,
	Connector_Output,
	Connector_Setting,
	Connector_Link,
};

class CBoxProxy
{
public:

	CBoxProxy(const IBox& rBox)
		:m_pBox(NULL)
		,m_pConstBox(&rBox)
		,m_i32XCenter(0)
		,m_i32YCenter(0)
		,m_i32Width(0)
		,m_i32Height(0)
	{
		if(m_pConstBox)
		{
			sscanf((const char*)m_pConstBox->getAttributeValue(AT_AttributeId_BoxCenterPosition), "%i %i", &m_i32XCenter, &m_i32YCenter);
			sscanf((const char*)m_pConstBox->getAttributeValue(AT_AttributeId_BoxSize), "%i %i", &m_i32Width, &m_i32Height);
		}
	}

	CBoxProxy(IScenario& rScenario, const CIdentifier& rBoxIdentifier)
		:m_pBox(rScenario.getBoxDetails(rBoxIdentifier))
		,m_pConstBox(rScenario.getBoxDetails(rBoxIdentifier))
		,m_i32XCenter(0)
		,m_i32YCenter(0)
		,m_i32Width(0)
		,m_i32Height(0)
	{
		if(m_pConstBox)
		{
			sscanf((const char*)m_pConstBox->getAttributeValue(AT_AttributeId_BoxCenterPosition), "%i %i", &m_i32XCenter, &m_i32YCenter);
			sscanf((const char*)m_pConstBox->getAttributeValue(AT_AttributeId_BoxSize), "%i %i", &m_i32Width, &m_i32Height);
		}
	}

	virtual ~CBoxProxy(void)
	{
		if(m_pBox)
		{
			char l_sSize[1024];
			sprintf(l_sSize, "%i %i", m_i32Width, m_i32Height);

			if(m_pBox->hasAttribute(AT_AttributeId_BoxSize))
			{
				m_pBox->setAttributeValue(AT_AttributeId_BoxSize, l_sSize);
			}
			else
			{
				m_pBox->addAttribute(AT_AttributeId_BoxSize, l_sSize);
			}

			char l_sCenter[1024];
			sprintf(l_sCenter, "%i %i", m_i32XCenter, m_i32YCenter);

			if(m_pBox->hasAttribute(AT_AttributeId_BoxCenterPosition))
			{
				m_pBox->setAttributeValue(AT_AttributeId_BoxCenterPosition, l_sCenter);
			}
			else
			{
				m_pBox->addAttribute(AT_AttributeId_BoxCenterPosition, l_sCenter);
			}
		}
	}

	operator IBox* (void)
	{
		return m_pBox;
	}

	operator const IBox* (void)
	{
		return m_pConstBox;
	}

	int32 getWidth(void) const { return m_i32Width; }
	int32 getHeight(void) const { return m_i32Height; }
	int32 getXCenter(void) const { return m_i32XCenter; }
	int32 getYCenter(void) const { return m_i32YCenter; }

	void setSize(int32 i32Width, int32 i32Height)
	{
		m_i32Width=i32Width;
		m_i32Height=i32Height;
	}

	void setCenter(int32 i32XCenter, int32 i32YCenter)
	{
		m_i32XCenter=i32XCenter;
		m_i32YCenter=i32YCenter;
	}

protected:

	IBox* m_pBox;
	const IBox* m_pConstBox;
	int32 m_i32XCenter;
	int32 m_i32YCenter;
	int32 m_i32Width;
	int32 m_i32Height;
};

class CLinkProxy
{
public:

	CLinkProxy(const ILink& rLink)
		:m_pLink(NULL)
		,m_pConstLink(&rLink)
		,m_i32XSource(0)
		,m_i32YSource(0)
		,m_i32XTarget(0)
		,m_i32YTarget(0)
	{
		if(m_pConstLink)
		{
			sscanf((const char*)m_pConstLink->getAttributeValue(AT_AttributeId_LinkSourcePosition), "%i %i", &m_i32XSource, &m_i32YSource);
			sscanf((const char*)m_pConstLink->getAttributeValue(AT_AttributeId_LinkTargetPosition), "%i %i", &m_i32XTarget, &m_i32YTarget);
		}
	}

	CLinkProxy(IScenario& rScenario, const CIdentifier& rLinkIdentifier)
		:m_pLink(rScenario.getLinkDetails(rLinkIdentifier))
		,m_pConstLink(rScenario.getLinkDetails(rLinkIdentifier))
		,m_i32XSource(0)
		,m_i32YSource(0)
		,m_i32XTarget(0)
		,m_i32YTarget(0)
	{
		if(m_pConstLink)
		{
			sscanf((const char*)m_pConstLink->getAttributeValue(AT_AttributeId_LinkSourcePosition), "%i %i", &m_i32XSource, &m_i32YSource);
			sscanf((const char*)m_pConstLink->getAttributeValue(AT_AttributeId_LinkTargetPosition), "%i %i", &m_i32XTarget, &m_i32YTarget);
		}
	}

	virtual ~CLinkProxy(void)
	{
		if(m_pLink)
		{
			char l_sSource[1024];
			sprintf(l_sSource, "%i %i", m_i32XSource, m_i32YSource);

			if(m_pLink->hasAttribute(AT_AttributeId_LinkSourcePosition))
			{
				m_pLink->setAttributeValue(AT_AttributeId_LinkSourcePosition, l_sSource);
			}
			else
			{
				m_pLink->addAttribute(AT_AttributeId_LinkSourcePosition, l_sSource);
			}

			char l_sTarget[1024];
			sprintf(l_sTarget, "%i %i", m_i32XTarget, m_i32YTarget);

			if(m_pLink->hasAttribute(AT_AttributeId_LinkTargetPosition))
			{
				m_pLink->setAttributeValue(AT_AttributeId_LinkTargetPosition, l_sTarget);
			}
			else
			{
				m_pLink->addAttribute(AT_AttributeId_LinkTargetPosition, l_sTarget);
			}
		}
	}

	operator ILink* (void)
	{
		return m_pLink;
	}

	operator const ILink* (void)
	{
		return m_pConstLink;
	}

	int32 getXSource(void) { return m_i32XSource; }
	int32 getYSource(void) { return m_i32YSource; }
	int32 getXTarget(void) { return m_i32XTarget; }
	int32 getYTarget(void) { return m_i32YTarget; }

	void setSource(int32 i32XSource, int32 i32YSource)
	{
		m_i32XSource=i32XSource;
		m_i32YSource=i32YSource;
	}

	void setTarget(int32 i32XTarget, int32 i32YTarget)
	{
		m_i32XTarget=i32XTarget;
		m_i32YTarget=i32YTarget;
	}

protected:

	ILink* m_pLink;
	const ILink* m_pConstLink;
	int32 m_i32XSource;
	int32 m_i32YSource;
	int32 m_i32XTarget;
	int32 m_i32YTarget;
};

class CLinkPositionSetterEnum : virtual public IScenario::ILinkEnum
{
public:

	CLinkPositionSetterEnum(uint32 ui32ConnectorType, vector<pair<int32, int32> >& rPosition)
		:m_ui32ConnectorType(ui32ConnectorType)
		,m_rPosition(rPosition)
	{
	}

	virtual boolean callback(const IScenario& rScenario, ILink& rLink)
	{
		char l_sPosition[1024];
		switch(m_ui32ConnectorType)
		{
			case Connector_Input:
				sprintf(l_sPosition, "%i %i", m_rPosition[rLink.getTargetBoxInputIndex()].first, m_rPosition[rLink.getTargetBoxInputIndex()].second);
				if(rLink.hasAttribute(AT_AttributeId_LinkTargetPosition))
					rLink.setAttributeValue(AT_AttributeId_LinkTargetPosition, l_sPosition);
				else
					rLink.addAttribute(AT_AttributeId_LinkTargetPosition, l_sPosition);
				break;

			case Connector_Output:
				sprintf(l_sPosition, "%i %i", m_rPosition[rLink.getSourceBoxOutputIndex()].first, m_rPosition[rLink.getSourceBoxOutputIndex()].second);
				if(rLink.hasAttribute(AT_AttributeId_LinkSourcePosition))
					rLink.setAttributeValue(AT_AttributeId_LinkSourcePosition, l_sPosition);
				else
					rLink.addAttribute(AT_AttributeId_LinkSourcePosition, l_sPosition);
				break;
		}
		return true;
	}

protected:

	uint32 m_ui32ConnectorType;
	vector<pair<int32, int32> > m_rPosition;
};

map<uint32, ::GdkColor> g_vColors;

::GtkTargetEntry g_vTargetEntry[]= {
	{ "STRING", 0, 0 },
	{ "text/plain", 0, 0 } };

// ------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------

class CInterfacedScenario : virtual public IScenario::IBoxEnum, virtual public IScenario::ILinkEnum
{
public:

	class SInterfacedObject
	{
	public:

		SInterfacedObject(void)
			:m_ui32ConnectorType(Connector_None)
			,m_ui32ConnectorIndex(0)
		{
		}

		SInterfacedObject(const CIdentifier& rId)
			:m_oIdentifier(rId)
			,m_ui32ConnectorType(Connector_None)
			,m_ui32ConnectorIndex(0)
		{
		}

		SInterfacedObject(const CIdentifier& rId, const uint32 ui32ConnectorType, const uint32 ui32ConnectorIndex)
			:m_oIdentifier(rId)
			,m_ui32ConnectorType(ui32ConnectorType)
			,m_ui32ConnectorIndex(ui32ConnectorIndex)
		{
		}

		CIdentifier m_oIdentifier;
		uint32 m_ui32ConnectorType;
		uint32 m_ui32ConnectorIndex;
	};

	CInterfacedScenario(IKernel& rKernel, IScenario& rScenario, CIdentifier& rScenarioIdentifier, ::GtkNotebook& rNotebook)
		:m_rKernel(rKernel)
		,m_rScenario(rScenario)
		,m_oScenarioIdentifier(rScenarioIdentifier)
		,m_rNotebook(rNotebook)
		,m_pPlayer(NULL)
		,m_pStencilBuffer(NULL)
		,m_bHasFileName(false)
		,m_bHasBeenModified(false)
		,m_bButtonPressed(false)
		,m_i32ViewOffsetX(0)
		,m_i32ViewOffsetY(0)
	{
		m_pGladeDummyScenarioNotebookTitle=glade_xml_new(AT_GUI_File, "dummy_scenario_notebook_title", NULL);
		m_pGladeDummyScenarioNotebookClient=glade_xml_new(AT_GUI_File, "dummy_scenario_notebook_client", NULL);
		m_pNotebookPageTitle=glade_xml_get_widget(m_pGladeDummyScenarioNotebookTitle, "dummy_scenario_notebook_title");
		m_pNotebookPageContent=glade_xml_get_widget(m_pGladeDummyScenarioNotebookClient, "dummy_scenario_notebook_client");
		gtk_widget_ref(m_pNotebookPageTitle);
		gtk_widget_ref(m_pNotebookPageContent);
		gtk_widget_unparent(m_pNotebookPageTitle);
		gtk_widget_unparent(m_pNotebookPageContent);
		gtk_notebook_append_page(&m_rNotebook, m_pNotebookPageContent, m_pNotebookPageTitle);
		gtk_widget_unref(m_pNotebookPageContent);
		gtk_widget_unref(m_pNotebookPageTitle);

		m_pScenarioDrawingArea=GTK_DRAWING_AREA(glade_xml_get_widget(m_pGladeDummyScenarioNotebookClient, "scenario_drawing_area"));
		gtk_drag_dest_set(GTK_WIDGET(m_pScenarioDrawingArea), GTK_DEST_DEFAULT_ALL, g_vTargetEntry, sizeof(g_vTargetEntry)/sizeof(::GtkTargetEntry), GDK_ACTION_COPY);
		g_signal_connect(G_OBJECT(m_pScenarioDrawingArea), "expose_event", G_CALLBACK(scenario_drawing_area_expose_cb), this);
		g_signal_connect(G_OBJECT(m_pScenarioDrawingArea), "drag_data_received", G_CALLBACK(scenario_drawing_area_drag_data_received_cb), this);
		g_signal_connect(G_OBJECT(m_pScenarioDrawingArea), "motion_notify_event", G_CALLBACK(scenario_drawing_area_motion_notify_cb), this);
		g_signal_connect(G_OBJECT(m_pScenarioDrawingArea), "button_press_event", G_CALLBACK(scenario_drawing_area_button_pressed_cb), this);
		g_signal_connect(G_OBJECT(m_pScenarioDrawingArea), "button_release_event", G_CALLBACK(scenario_drawing_area_button_released_cb), this);
	}

	virtual ~CInterfacedScenario(void)
	{
		if(m_pStencilBuffer) g_object_unref(m_pStencilBuffer);

		gtk_notebook_remove_page(
			&m_rNotebook,
			gtk_notebook_page_num(&m_rNotebook, m_pNotebookPageContent));
	}

	virtual void updateLabel(void)
	{
		::GtkLabel* l_pTitleLabel=GTK_LABEL(glade_xml_get_widget(m_pGladeDummyScenarioNotebookTitle, "scenario_label"));
		string l_sLabel;
		l_sLabel+=m_bHasBeenModified?"*":"";
		l_sLabel+=" ";
		l_sLabel+=m_bHasFileName?m_sFileName.substr(m_sFileName.rfind('/')+1):"(untitled)";
		l_sLabel+=" ";
		l_sLabel+=m_bHasBeenModified?"*":"";
		gtk_label_set_text(l_pTitleLabel, l_sLabel.c_str());
	}

	virtual boolean getTextSize(::GtkWidget* pWidget, const char* sText, int32* pXSize, int32* pYSize)
	{
		::PangoContext* l_pPangoContext=NULL;
		::PangoLayout* l_pPangoLayout=NULL;
		::PangoRectangle l_oPangoRectangle;
		l_pPangoContext=gtk_widget_get_pango_context(pWidget);
		l_pPangoLayout=pango_layout_new(l_pPangoContext);
		pango_layout_set_text(l_pPangoLayout, sText, -1);
		pango_layout_get_pixel_extents(l_pPangoLayout, NULL, &l_oPangoRectangle);
		*pXSize=l_oPangoRectangle.width;
		*pYSize=l_oPangoRectangle.height;
		g_object_unref(l_pPangoContext);
		return true;
	}

#define updateStencilIndex(id,stencilgc) { id++; ::GdkColor sc={0, (guint16)((id&0xff0000)>>8), (guint16)(id&0xff00), (guint16)((id&0xff)<<8) }; gdk_gc_set_rgb_fg_color(stencilgc, &sc); }

	virtual boolean callback(const IScenario& rScenario, IBox& rBox) // IScenario::IBoxEnum callback !
	{
		::GtkWidget* l_pWidget=GTK_WIDGET(m_pScenarioDrawingArea);
		::GdkGC* l_pStencilGC=gdk_gc_new(GDK_DRAWABLE(m_pStencilBuffer));
		::GdkGC* l_pDrawGC=gdk_gc_new(l_pWidget->window);

		vector<pair<int32, int32> > l_vInputPosition;
		vector<pair<int32, int32> > l_vOutputPosition;

		OpenViBE::uint32 i;
		const int xMargin=16;
		const int yMargin=16;
		const int iCircleMargin=4;
		const int iCircleSize=9;
		const int iCircleSpace=8;

		CBoxProxy l_oBoxProxy(rBox);
		int xSize=l_oBoxProxy.getWidth()+xMargin*2;
		int ySize=l_oBoxProxy.getHeight()+yMargin*2;
		int xStart=l_oBoxProxy.getXCenter()+m_i32ViewOffsetX-(xSize>>1);
		int yStart=l_oBoxProxy.getYCenter()+m_i32ViewOffsetY-(ySize>>1);

		updateStencilIndex(m_ui32InterfacedObjectId, l_pStencilGC);
		gdk_draw_rectangle(
			GDK_DRAWABLE(m_pStencilBuffer),
			l_pStencilGC,
			TRUE,
			xStart, yStart, xSize, ySize);
		m_vInterfacedObject[m_ui32InterfacedObjectId]=SInterfacedObject(rBox.getIdentifier());

		gdk_gc_set_rgb_fg_color(l_pDrawGC, &g_vColors[m_oCurrentObject.m_oIdentifier==rBox.getIdentifier()?Color_BoxBackgroundSelected:Color_BoxBackground]);
		gdk_draw_rectangle(
			l_pWidget->window,
			l_pDrawGC,
			TRUE,
			xStart, yStart, xSize, ySize);
		gdk_gc_set_rgb_fg_color(l_pDrawGC, &g_vColors[m_oCurrentObject.m_oIdentifier==rBox.getIdentifier()?Color_BoxBorderSelected:Color_BoxBorder]);
		gdk_draw_rectangle(
			l_pWidget->window,
			l_pDrawGC,
			FALSE,
			xStart, yStart, xSize, ySize);

		for(i=0; i<rBox.getInputCount(); i++)
		{
			updateStencilIndex(m_ui32InterfacedObjectId, l_pStencilGC);
			gdk_draw_arc(
				GDK_DRAWABLE(m_pStencilBuffer),
				l_pStencilGC,
				TRUE,
				xStart+i*(iCircleSpace+iCircleSize)+iCircleMargin, yStart+iCircleMargin, iCircleSize, iCircleSize,
				0*64, 360*64);
			m_vInterfacedObject[m_ui32InterfacedObjectId]=SInterfacedObject(rBox.getIdentifier(), Connector_Input, i);

			gdk_gc_set_rgb_fg_color(l_pDrawGC, &g_vColors[Color_BoxInputBackground]);
			gdk_draw_arc(
				l_pWidget->window,
				l_pDrawGC,
				TRUE,
				xStart+i*(iCircleSpace+iCircleSize)+iCircleMargin, yStart+iCircleMargin, iCircleSize, iCircleSize,
				0*64, 360*64);
			gdk_gc_set_rgb_fg_color(l_pDrawGC, &g_vColors[Color_BoxInputBorder]);
			gdk_draw_arc(
				l_pWidget->window,
				l_pDrawGC,
				FALSE,
				xStart+i*(iCircleSpace+iCircleSize)+iCircleMargin, yStart+iCircleMargin, iCircleSize, iCircleSize,
				0*64, 360*64);

			l_vInputPosition.push_back(pair<int32, int32>(xStart+i*(iCircleSpace+iCircleSize)+iCircleMargin+(iCircleSize>>1)-m_i32ViewOffsetX, yStart+iCircleMargin+(iCircleSize>>1)-m_i32ViewOffsetY));
		}

		for(i=0; i<rBox.getOutputCount(); i++)
		{
			updateStencilIndex(m_ui32InterfacedObjectId, l_pStencilGC);
			gdk_draw_arc(
				GDK_DRAWABLE(m_pStencilBuffer),
				l_pStencilGC,
				TRUE,
				xStart+i*(iCircleSpace+iCircleSize)+iCircleMargin, yStart+ySize-iCircleSize-iCircleMargin, iCircleSize, iCircleSize,
				0*64, 360*64);
			m_vInterfacedObject[m_ui32InterfacedObjectId]=SInterfacedObject(rBox.getIdentifier(), Connector_Output, i);

			gdk_gc_set_rgb_fg_color(l_pDrawGC, &g_vColors[Color_BoxOutputBackground]);
			gdk_draw_arc(
				l_pWidget->window,
				l_pDrawGC,
				TRUE,
				xStart+i*(iCircleSpace+iCircleSize)+iCircleMargin, yStart+ySize-iCircleSize-iCircleMargin, iCircleSize, iCircleSize,
				0*64, 360*64);
			gdk_gc_set_rgb_fg_color(l_pDrawGC, &g_vColors[Color_BoxOutputBorder]);
			gdk_draw_arc(
				l_pWidget->window,
				l_pDrawGC,
				FALSE,
				xStart+i*(iCircleSpace+iCircleSize)+iCircleMargin, yStart+ySize-iCircleSize-iCircleMargin, iCircleSize, iCircleSize,
				0*64, 360*64);

			l_vOutputPosition.push_back(pair<int32, int32>(xStart+i*(iCircleSpace+iCircleSize)+iCircleMargin+(iCircleSize>>1)-m_i32ViewOffsetX, yStart+ySize-iCircleSize-iCircleMargin+(iCircleSize>>1)-m_i32ViewOffsetY));
		}

		for(i=0; i<rBox.getSettingCount(); i++)
		{
			::GdkPoint l_vPoint[4];
			l_vPoint[0].x=iCircleSize>>1;
			l_vPoint[0].y=0;
			l_vPoint[1].x=iCircleSize-1;
			l_vPoint[1].y=iCircleSize>>1;
			l_vPoint[2].x=iCircleSize>>1;
			l_vPoint[2].y=iCircleSize-1;
			l_vPoint[3].x=0;
			l_vPoint[3].y=iCircleSize>>1;
			for(int j=0; j<4; j++)
			{
				l_vPoint[j].x+=xStart+xSize-iCircleSize-iCircleMargin+1;
				l_vPoint[j].y+=yStart+iCircleMargin+i*(iCircleSpace+iCircleSize);
			}

			updateStencilIndex(m_ui32InterfacedObjectId, l_pStencilGC);
			gdk_draw_polygon(
				GDK_DRAWABLE(m_pStencilBuffer),
				l_pStencilGC,
				TRUE,
				l_vPoint,
				4);
			m_vInterfacedObject[m_ui32InterfacedObjectId]=SInterfacedObject(rBox.getIdentifier(), Connector_Setting, i);

			gdk_gc_set_rgb_fg_color(l_pDrawGC, &g_vColors[Color_BoxSettingBackground]);
			gdk_draw_polygon(
				l_pWidget->window,
				l_pDrawGC,
				TRUE,
				l_vPoint,
				4);
			gdk_gc_set_rgb_fg_color(l_pDrawGC, &g_vColors[Color_BoxSettingBorder]);
			gdk_draw_polygon(
				l_pWidget->window,
				l_pDrawGC,
				FALSE,
				l_vPoint,
				4);
		}

		::PangoContext* l_pPangoContext=NULL;
		::PangoLayout* l_pPangoLayout=NULL;
		l_pPangoContext=gtk_widget_get_pango_context(l_pWidget);
		l_pPangoLayout=pango_layout_new(l_pPangoContext);
		pango_layout_set_text(l_pPangoLayout, rBox.getName(), -1);
		gdk_draw_layout(
			l_pWidget->window,
			l_pWidget->style->text_gc[GTK_WIDGET_STATE(l_pWidget)],
			xStart+xMargin, yStart+yMargin, l_pPangoLayout);
		g_object_unref(l_pPangoLayout);

		g_object_unref(l_pDrawGC);
		g_object_unref(l_pStencilGC);

		CLinkPositionSetterEnum l_oLinkPositionSetterInput(Connector_Input, l_vInputPosition);
		CLinkPositionSetterEnum l_oLinkPositionSetterOutput(Connector_Output, l_vOutputPosition);
		rScenario.enumerateLinksToBox(l_oLinkPositionSetterInput, rBox.getIdentifier());
		rScenario.enumerateLinksFromBox(l_oLinkPositionSetterOutput, rBox.getIdentifier());

		return true;
	}

	virtual boolean callback(const IScenario& rScenario, ILink& rLink) // IScenario::ILinkEnum callback !
	{
		::GtkWidget* l_pWidget=GTK_WIDGET(m_pScenarioDrawingArea);
		::GdkGC* l_pStencilGC=gdk_gc_new(GDK_DRAWABLE(m_pStencilBuffer));
		::GdkGC* l_pDrawGC=gdk_gc_new(l_pWidget->window);

		const int xMargin=16;
		const int yMargin=16;
		const int iCircleMargin=4;
		const int iCircleSize=9;
		const int iCircleSpace=8;

		CLinkProxy l_oLinkProxy(rLink);

		updateStencilIndex(m_ui32InterfacedObjectId, l_pStencilGC);
		gdk_gc_set_rgb_fg_color(l_pDrawGC, &g_vColors[m_oCurrentObject.m_oIdentifier==rLink.getIdentifier()?Color_LinkSelected:Color_Link]);
		gdk_draw_line(
			GDK_DRAWABLE(m_pStencilBuffer),
			l_pStencilGC,
			l_oLinkProxy.getXSource()+m_i32ViewOffsetX, l_oLinkProxy.getYSource()+m_i32ViewOffsetY,
			l_oLinkProxy.getXTarget()+m_i32ViewOffsetX, l_oLinkProxy.getYTarget()+m_i32ViewOffsetY);
		gdk_draw_line(
			l_pWidget->window,
			l_pDrawGC,
			l_oLinkProxy.getXSource()+m_i32ViewOffsetX, l_oLinkProxy.getYSource()+m_i32ViewOffsetY,
			l_oLinkProxy.getXTarget()+m_i32ViewOffsetX, l_oLinkProxy.getYTarget()+m_i32ViewOffsetY);
		m_vInterfacedObject[m_ui32InterfacedObjectId]=SInterfacedObject(rLink.getIdentifier(), Connector_Link, 0);

		g_object_unref(l_pDrawGC);
		g_object_unref(l_pStencilGC);

		return true;
	}

	void prepareStencil(void)
	{
		gint x,y;

		gdk_window_get_size(GTK_WIDGET(m_pScenarioDrawingArea)->window, &x, &y);
		if(m_pStencilBuffer) g_object_unref(m_pStencilBuffer);
		m_pStencilBuffer=gdk_pixmap_new(NULL, x, y, 24);

		m_ui32InterfacedObjectId=0;
		m_vInterfacedObject.clear();
	}

#undef updateStencilIndex

	void scenarioDrawingAreaExposeCB(::GdkEventExpose* pEvent)
	{
		prepareStencil();
		m_rScenario.enumerateBoxes(*this);
		m_rScenario.enumerateLinks(*this);
	}
	void scenarioDrawingAreaDragDataReceivedCB(::GdkDragContext* pDragContext, gint iX, gint iY, ::GtkSelectionData* pSelectionData, guint uiInfo, guint uiT)
	{
		// cout << "scenarioDrawingAreaDragDataReceivedCB [" << gtk_selection_data_get_text(pSelectionData) << "]" << endl;
		CIdentifier l_oBoxIdentifier;
		CIdentifier l_oBoxAlgorithmClassIdentifier;
		l_oBoxAlgorithmClassIdentifier.fromString((const char*)gtk_selection_data_get_text(pSelectionData));
		if(l_oBoxAlgorithmClassIdentifier!=OV_UndefinedIdentifier)
		{
			int32 l_i32Width=0;
			int32 l_i32Height=0;
			m_rScenario.addBox(l_oBoxAlgorithmClassIdentifier, l_oBoxIdentifier);
			CBoxProxy l_oBoxProxy(m_rScenario, l_oBoxIdentifier);
			getTextSize(GTK_WIDGET(m_pScenarioDrawingArea), ((IBox*)l_oBoxProxy)->getName(), &l_i32Width, &l_i32Height);
			l_oBoxProxy.setCenter(iX-m_i32ViewOffsetX, iY-m_i32ViewOffsetY);
			l_oBoxProxy.setSize(l_i32Width, l_i32Height);

			m_bHasBeenModified=true;
			updateLabel();
		}

		m_f64MouseX=iX;
		m_f64MouseY=iY;
	}
	void scenarioDrawingAreaMotionNotifyCB(::GtkWidget* pWidget, ::GdkEventMotion* pEvent)
	{
		// cout << "scenarioDrawingAreaMotionNotifyCB" << endl;
		if(m_bButtonPressed)
		{
			if(m_oCurrentObject.m_oIdentifier==OV_UndefinedIdentifier)
			{
				m_i32ViewOffsetX+=(int32)(pEvent->x-m_f64MouseX);
				m_i32ViewOffsetY+=(int32)(pEvent->y-m_f64MouseY);
				gdk_window_invalidate_rect(
					GTK_WIDGET(m_pScenarioDrawingArea)->window,
					NULL,
					true);
			}
			else if(m_oCurrentObject.m_ui32ConnectorType==Connector_None)
			{
				CBoxProxy l_oBoxProxy(m_rScenario, m_oCurrentObject.m_oIdentifier);
				l_oBoxProxy.setCenter(
					l_oBoxProxy.getXCenter()+(int32)(pEvent->x-m_f64MouseX),
					l_oBoxProxy.getYCenter()+(int32)(pEvent->y-m_f64MouseY));
				gdk_window_invalidate_rect(
					GTK_WIDGET(m_pScenarioDrawingArea)->window,
					NULL,
					true);
			}
		}
		m_f64MouseX=pEvent->x;
		m_f64MouseY=pEvent->y;
	}
	void scenarioDrawingAreaButtonPressedCB(::GtkWidget* pWidget, ::GdkEventButton* pEvent)
	{
		// cout << "scenarioDrawingAreaButtonPressedCB" << endl;
		m_bButtonPressed|=((pEvent->type==GDK_BUTTON_PRESS)&&(pEvent->button==1));

		::GdkPixbuf* l_pPixbuf=gdk_pixbuf_get_from_drawable(NULL, GDK_DRAWABLE(m_pStencilBuffer), NULL, (int)pEvent->x, (int)pEvent->y, 0, 0, 1, 1);
		uint32 l_ui32InterfacedObjectId=(gdk_pixbuf_get_pixels(l_pPixbuf)[0]<<16)+(gdk_pixbuf_get_pixels(l_pPixbuf)[1]<<8)+(gdk_pixbuf_get_pixels(l_pPixbuf)[2]);
		m_oCurrentObject=m_vInterfacedObject[l_ui32InterfacedObjectId];
		g_object_unref(l_pPixbuf);

		gdk_window_invalidate_rect(
			GTK_WIDGET(m_pScenarioDrawingArea)->window,
			NULL,
			true);
	}
	void scenarioDrawingAreaButtonReleasedCB(::GtkWidget* pWidget, ::GdkEventButton* pEvent)
	{
		// cout << "scenarioDrawingAreaButtonReleasedCB" << endl;
		m_bButtonPressed&=!((pEvent->type==GDK_BUTTON_RELEASE)&&(pEvent->button==1));

		::GdkPixbuf* l_pPixbuf=gdk_pixbuf_get_from_drawable(NULL, GDK_DRAWABLE(m_pStencilBuffer), NULL, (int)pEvent->x, (int)pEvent->y, 0, 0, 1, 1);
		uint32 l_ui32InterfacedObjectId=(gdk_pixbuf_get_pixels(l_pPixbuf)[0]<<16)+(gdk_pixbuf_get_pixels(l_pPixbuf)[1]<<8)+(gdk_pixbuf_get_pixels(l_pPixbuf)[2]);
		SInterfacedObject l_oCurrentObject=m_vInterfacedObject[l_ui32InterfacedObjectId];
		g_object_unref(l_pPixbuf);

		if(m_oCurrentObject.m_ui32ConnectorType==Connector_Output && l_oCurrentObject.m_ui32ConnectorType==Connector_Input)
		{
			CIdentifier l_oLinkIdentifier;
			m_rScenario.connect(
				m_oCurrentObject.m_oIdentifier,
				m_oCurrentObject.m_ui32ConnectorIndex,
				l_oCurrentObject.m_oIdentifier,
				l_oCurrentObject.m_ui32ConnectorIndex,
				l_oLinkIdentifier);
			gdk_window_invalidate_rect(
				GTK_WIDGET(m_pScenarioDrawingArea)->window,
				NULL,
				true);
		}
	}

	static void scenario_drawing_area_expose_cb(::GtkWidget* pWidget, ::GdkEventExpose* pEvent, gpointer pUserData)
	{
		static_cast<CInterfacedScenario*>(pUserData)->scenarioDrawingAreaExposeCB(pEvent);
	}
	static void scenario_drawing_area_drag_data_received_cb(::GtkWidget* pWidget, ::GdkDragContext* pDragContext, gint iX, gint iY, ::GtkSelectionData* pSelectionData, guint uiInfo, guint uiT, gpointer pUserData)
	{
		static_cast<CInterfacedScenario*>(pUserData)->scenarioDrawingAreaDragDataReceivedCB(pDragContext, iX, iY, pSelectionData, uiInfo, uiT);
	}
	static void scenario_drawing_area_motion_notify_cb(::GtkWidget* pWidget, ::GdkEventMotion* pEvent, gpointer pUserData)
	{
		static_cast<CInterfacedScenario*>(pUserData)->scenarioDrawingAreaMotionNotifyCB(pWidget, pEvent);
	}
	static void scenario_drawing_area_button_pressed_cb(::GtkWidget* pWidget, ::GdkEventButton* pEvent, gpointer pUserData)
	{
		static_cast<CInterfacedScenario*>(pUserData)->scenarioDrawingAreaButtonPressedCB(pWidget, pEvent);
	}
	static void scenario_drawing_area_button_released_cb(::GtkWidget* pWidget, ::GdkEventButton* pEvent, gpointer pUserData)
	{
		static_cast<CInterfacedScenario*>(pUserData)->scenarioDrawingAreaButtonReleasedCB(pWidget, pEvent);
	}

public:

	CIdentifier m_oScenarioIdentifier;
	IKernel& m_rKernel;
	IScenario& m_rScenario;
	IPlayer* m_pPlayer;
	::GtkNotebook& m_rNotebook;
	::GladeXML* m_pGladeDummyScenarioNotebookTitle;
	::GladeXML* m_pGladeDummyScenarioNotebookClient;
	::GtkWidget* m_pNotebookPageTitle;
	::GtkWidget* m_pNotebookPageContent;
	::GtkDrawingArea* m_pScenarioDrawingArea;
	::GdkPixmap* m_pStencilBuffer;
	boolean m_bHasFileName;
	boolean m_bHasBeenModified;
	boolean m_bButtonPressed;
	string m_sFileName;
	float64 m_f64MouseX;
	float64 m_f64MouseY;
	int32 m_i32ViewOffsetX;
	int32 m_i32ViewOffsetY;

	uint32 m_ui32InterfacedObjectId;
	map<uint32, SInterfacedObject> m_vInterfacedObject;
	SInterfacedObject m_oCurrentObject;
};

// ------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------

class CApplication
{
public:

	CApplication(IKernel* pKernel)
		:m_pKernel(pKernel)
		,m_pPluginManager(NULL)
		,m_pScenarioManager(NULL)
		,m_pGladeInterface(NULL)
		,m_pMainWindow(NULL)
		,m_pScenarioNotebook(NULL)
		,m_pBoxAlgorithmNotebook(NULL)
		,m_pBoxAlgorithmTreeModel(NULL)
		,m_pBoxAlgorithmTreeView(NULL)
	{
		m_pPluginManager=&m_pKernel->getContext()->getPluginManager();
		m_pScenarioManager=&m_pKernel->getContext()->getScenarioManager();
	}

	void init(void)
	{
		// Loads main interface
		m_pGladeInterface=glade_xml_new(AT_GUI_File, NULL, NULL);
		m_pMainWindow=glade_xml_get_widget(m_pGladeInterface, "openvibe");

		// Connects menu actions
		g_signal_connect(G_OBJECT(glade_xml_get_widget(m_pGladeInterface, "menu_new")),     "activate", G_CALLBACK(new_scenario_cb),     this);
		g_signal_connect(G_OBJECT(glade_xml_get_widget(m_pGladeInterface, "menu_open")),    "activate", G_CALLBACK(open_scenario_cb),    this);
		g_signal_connect(G_OBJECT(glade_xml_get_widget(m_pGladeInterface, "menu_save")),    "activate", G_CALLBACK(save_scenario_cb),    this);
		g_signal_connect(G_OBJECT(glade_xml_get_widget(m_pGladeInterface, "menu_save_as")), "activate", G_CALLBACK(save_scenario_as_cb), this);
		g_signal_connect(G_OBJECT(glade_xml_get_widget(m_pGladeInterface, "menu_close")),   "activate", G_CALLBACK(close_scenario_cb),   this);
		g_signal_connect(G_OBJECT(glade_xml_get_widget(m_pGladeInterface, "button_run")),   "clicked",  G_CALLBACK(run_scenario_cb),     this);
		g_signal_connect(G_OBJECT(glade_xml_get_widget(m_pGladeInterface, "button_stop")),  "clicked",  G_CALLBACK(stop_scenario_cb),    this);
		g_signal_connect(G_OBJECT(glade_xml_get_widget(m_pGladeInterface, "button_step")),  "clicked",  G_CALLBACK(step_scenario_cb),    this);
		g_signal_connect(G_OBJECT(glade_xml_get_widget(m_pGladeInterface, "button_pause")), "clicked",  G_CALLBACK(pause_scenario_cb),   this);

		// Prepares main notebooks
		m_pScenarioNotebook=GTK_NOTEBOOK(glade_xml_get_widget(m_pGladeInterface, "scenario_notebook"));
		m_pBoxAlgorithmNotebook=GTK_NOTEBOOK(glade_xml_get_widget(m_pGladeInterface, "box_algorithm_notebook"));
		gtk_notebook_remove_page(m_pScenarioNotebook, 0);
		newScenarioCB(NULL);

		// Prepares box algorithm view
		m_pBoxAlgorithmTreeView=GTK_TREE_VIEW(glade_xml_get_widget(m_pGladeInterface, "box_algorithm_tree"));
		::GtkTreeViewColumn* l_pTreeViewColumnName=gtk_tree_view_column_new();
		::GtkTreeViewColumn* l_pTreeViewColumnDesc=gtk_tree_view_column_new();
		::GtkCellRenderer* l_pCellRendererIcon=gtk_cell_renderer_pixbuf_new();
		::GtkCellRenderer* l_pCellRendererName=gtk_cell_renderer_text_new();
		::GtkCellRenderer* l_pCellRendererDesc=gtk_cell_renderer_text_new();
		gtk_tree_view_column_set_title(l_pTreeViewColumnName, "Name");
		gtk_tree_view_column_set_title(l_pTreeViewColumnDesc, "Description");
		gtk_tree_view_column_pack_start(l_pTreeViewColumnName, l_pCellRendererIcon, TRUE);
		gtk_tree_view_column_pack_start(l_pTreeViewColumnName, l_pCellRendererName, TRUE);
		gtk_tree_view_column_pack_start(l_pTreeViewColumnDesc, l_pCellRendererDesc, TRUE);
		gtk_tree_view_column_set_attributes(l_pTreeViewColumnName, l_pCellRendererIcon, "stock-id", BoxAlgorithm_StringStockIcon, NULL);
		gtk_tree_view_column_set_attributes(l_pTreeViewColumnName, l_pCellRendererName, "text", BoxAlgorithm_StringName, NULL);
		gtk_tree_view_column_set_attributes(l_pTreeViewColumnDesc, l_pCellRendererDesc, "text", BoxAlgorithm_StringShortDescription, NULL);
		gtk_tree_view_append_column(m_pBoxAlgorithmTreeView, l_pTreeViewColumnName);
		gtk_tree_view_append_column(m_pBoxAlgorithmTreeView, l_pTreeViewColumnDesc);

		// Prepares drag & drop for box creation
		gtk_drag_source_set(GTK_WIDGET(m_pBoxAlgorithmTreeView), GDK_BUTTON1_MASK, g_vTargetEntry, sizeof(g_vTargetEntry)/sizeof(::GtkTargetEntry), GDK_ACTION_COPY);
		g_signal_connect(
			G_OBJECT(m_pBoxAlgorithmTreeView),
			"drag_data_get",
			G_CALLBACK(drag_data_get_cb),
			this);

		// Prepares box algorithm model
		m_pBoxAlgorithmTreeModel=gtk_tree_store_new(5, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);
		gtk_tree_view_set_model(m_pBoxAlgorithmTreeView, GTK_TREE_MODEL(m_pBoxAlgorithmTreeModel));

		// Shows main window
		glade_xml_signal_autoconnect(m_pGladeInterface);
		gtk_widget_show(m_pMainWindow);
	}

	CInterfacedScenario* getCurrentInterfacedScenario(void)
	{
		uint32 l_ui32Index=(uint32)gtk_notebook_get_current_page(m_pScenarioNotebook);
		if(l_ui32Index<m_vInterfacedScenario.size())
		{
			return m_vInterfacedScenario[l_ui32Index];
		}
		return NULL;
	}

	void dragDataGetCB(::GtkWidget* pWidget, ::GdkDragContext* pDragContex, ::GtkSelectionData* pSelectionData, guint uiInfo, guint uiT)
	{
		::GtkTreeView* l_pTreeView=GTK_TREE_VIEW(glade_xml_get_widget(m_pGladeInterface, "box_algorithm_tree"));
		::GtkTreeSelection* l_pTreeSelection=gtk_tree_view_get_selection(l_pTreeView);
		::GtkTreeModel* l_pTreeModel=NULL;
		::GtkTreeIter l_oTreeIter;
		if(gtk_tree_selection_get_selected(l_pTreeSelection, &l_pTreeModel, &l_oTreeIter))
		{
			const char* l_sBoxAlgorithmIdentifier=NULL;
			gtk_tree_model_get(
				l_pTreeModel, &l_oTreeIter,
				BoxAlgorithm_StringIdentifier, &l_sBoxAlgorithmIdentifier,
				-1);
			if(l_sBoxAlgorithmIdentifier)
			{
				gtk_selection_data_set(
					pSelectionData,
					GDK_SELECTION_TYPE_STRING,
					8,
					(const guchar*)l_sBoxAlgorithmIdentifier,
					strlen(l_sBoxAlgorithmIdentifier)+1);
			}
		}
	}

	void newScenarioCB(::GtkMenuItem* pMenuItem)
	{
		CIdentifier l_oScenarioIdentifier;
		if(m_pScenarioManager->createScenario(l_oScenarioIdentifier))
		{
			IScenario& l_rScenario=m_pScenarioManager->getScenario(l_oScenarioIdentifier);
			CInterfacedScenario* l_pInterfacedScenario=new CInterfacedScenario(*m_pKernel, l_rScenario, l_oScenarioIdentifier, *m_pScenarioNotebook);
			l_pInterfacedScenario->updateLabel();
			gtk_notebook_set_current_page(m_pScenarioNotebook, gtk_notebook_get_n_pages(m_pScenarioNotebook)-1);
			m_vInterfacedScenario.push_back(l_pInterfacedScenario);
		}
	}
	void openScenarioCB(::GtkMenuItem* pMenuItem)
	{
		::GtkWidget* l_pWidgetDialogOpen=gtk_file_chooser_dialog_new(
			"Select scenario to open...",
			NULL,
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);
		if(gtk_dialog_run(GTK_DIALOG(l_pWidgetDialogOpen)))
		{
			char* l_sFileName=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(l_pWidgetDialogOpen));

			CIdentifier l_oScenarioIdentifier;
			if(m_pScenarioManager->createScenario(l_oScenarioIdentifier))
			{
				IScenario& l_rScenario=m_pScenarioManager->getScenario(l_oScenarioIdentifier);
				l_rScenario.load(l_sFileName, CIdentifier(0x440BF3AC, 0x2D960300));
				CInterfacedScenario* l_pInterfacedScenario=new CInterfacedScenario(*m_pKernel, l_rScenario, l_oScenarioIdentifier, *m_pScenarioNotebook);
				l_pInterfacedScenario->m_sFileName=l_sFileName;
				l_pInterfacedScenario->m_bHasFileName=true;
				l_pInterfacedScenario->m_bHasBeenModified=false;
				l_pInterfacedScenario->updateLabel();
				gtk_notebook_set_current_page(m_pScenarioNotebook, gtk_notebook_get_n_pages(m_pScenarioNotebook)-1);
				m_vInterfacedScenario.push_back(l_pInterfacedScenario);
			}

			g_free(l_sFileName);
		}
		gtk_widget_destroy(l_pWidgetDialogOpen);
	}
	void saveScenarioCB(::GtkMenuItem* pMenuItem)
	{
		CInterfacedScenario* l_pCurrentInterfacedScenario=getCurrentInterfacedScenario();
		if(!l_pCurrentInterfacedScenario)
		{
			return;
		}
		if(!l_pCurrentInterfacedScenario->m_bHasFileName)
		{
			saveScenarioAsCB(pMenuItem);
		}
		else
		{
			cout<<"[ INFO ] Exporting scenario..."<<endl;
			l_pCurrentInterfacedScenario->m_rScenario.save(l_pCurrentInterfacedScenario->m_sFileName.c_str(), CIdentifier(0x77075b3b, 0x3d632492));
			l_pCurrentInterfacedScenario->m_bHasFileName=true;
			l_pCurrentInterfacedScenario->m_bHasBeenModified=false;
			l_pCurrentInterfacedScenario->updateLabel();
		}
	}
	void saveScenarioAsCB(::GtkMenuItem* pMenuItem)
	{
		CInterfacedScenario* l_pCurrentInterfacedScenario=getCurrentInterfacedScenario();
		if(!l_pCurrentInterfacedScenario)
		{
			return;
		}
		::GtkWidget* l_pWidgetDialogSaveAs=gtk_file_chooser_dialog_new(
			"Select scenario to save...",
			NULL,
			GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
			NULL);
		// gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(l_pWidgetDialogSaveAs), true);
		if(l_pCurrentInterfacedScenario->m_bHasFileName)
		{
			gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(l_pWidgetDialogSaveAs), l_pCurrentInterfacedScenario->m_sFileName.c_str());
		}
		if(gtk_dialog_run(GTK_DIALOG(l_pWidgetDialogSaveAs)))
		{
			char* l_sFileName=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(l_pWidgetDialogSaveAs));

			l_pCurrentInterfacedScenario->m_sFileName=l_sFileName;
			l_pCurrentInterfacedScenario->m_bHasFileName=true;
			l_pCurrentInterfacedScenario->m_bHasBeenModified=false;
			saveScenarioCB(pMenuItem);

			g_free(l_sFileName);
		}
		gtk_widget_destroy(l_pWidgetDialogSaveAs);
	}
	void closeScenarioCB(::GtkMenuItem* pMenuItem)
	{
		CInterfacedScenario* l_pCurrentInterfacedScenario=getCurrentInterfacedScenario();
		vector<CInterfacedScenario*>::iterator i=m_vInterfacedScenario.begin();
		while(i!=m_vInterfacedScenario.end() && *i!=l_pCurrentInterfacedScenario) i++;
		if(i!=m_vInterfacedScenario.end())
		{
			CIdentifier l_oScenarioIdentifier=l_pCurrentInterfacedScenario->m_oScenarioIdentifier;
			delete l_pCurrentInterfacedScenario;
			m_pScenarioManager->releaseScenario(l_oScenarioIdentifier);
			m_vInterfacedScenario.erase(i);
		}
	}
	void runScenarioCB(::GtkButton* pButton)
	{
		CInterfacedScenario* l_pCurrentInterfacedScenario=getCurrentInterfacedScenario();
		if(l_pCurrentInterfacedScenario)
		{
			if(l_pCurrentInterfacedScenario->m_pPlayer)
			{
				g_idle_add(idle_scenario_step, l_pCurrentInterfacedScenario->m_pPlayer);
			}
		}
	}
	void stopScenarioCB(::GtkButton* pButton)
	{
		CInterfacedScenario* l_pCurrentInterfacedScenario=getCurrentInterfacedScenario();
		if(l_pCurrentInterfacedScenario)
		{
			if(l_pCurrentInterfacedScenario->m_pPlayer)
			{
				g_idle_remove_by_data(l_pCurrentInterfacedScenario->m_pPlayer);
				m_pKernel->getContext()->getObjectFactory().releaseObject(l_pCurrentInterfacedScenario->m_pPlayer);
				l_pCurrentInterfacedScenario->m_pPlayer=NULL;
			}

			CIdentifier l_oScenarioIdentifier=l_pCurrentInterfacedScenario->m_oScenarioIdentifier;
			l_pCurrentInterfacedScenario->m_pPlayer=dynamic_cast<IPlayer*>(m_pKernel->getContext()->getObjectFactory().createObject(OV_ClassId_Kernel_Player_Player));
			l_pCurrentInterfacedScenario->m_pPlayer->reset(m_pScenarioManager->getScenario(l_oScenarioIdentifier), m_pKernel->getContext()->getPluginManager());
		}
	}
	void pauseScenarioCB(::GtkButton* pButton)
	{
		CInterfacedScenario* l_pCurrentInterfacedScenario=getCurrentInterfacedScenario();
		if(l_pCurrentInterfacedScenario)
		{
			if(l_pCurrentInterfacedScenario->m_pPlayer)
			{
				g_idle_remove_by_data(l_pCurrentInterfacedScenario->m_pPlayer);
			}
		}
	}
	void stepScenarioCB(::GtkButton* pButton)
	{
		CInterfacedScenario* l_pCurrentInterfacedScenario=getCurrentInterfacedScenario();
		if(l_pCurrentInterfacedScenario)
		{
			if(l_pCurrentInterfacedScenario->m_pPlayer)
			{
				l_pCurrentInterfacedScenario->m_pPlayer->loop();
			}
		}
	}

	static void drag_data_get_cb(::GtkWidget* pWidget, ::GdkDragContext* pDragContex, ::GtkSelectionData* pSelectionData, guint uiInfo, guint uiT, gpointer pUserData)
	{
		// cout<<"drag_data_get_cb"<<endl;
		static_cast<CApplication*>(pUserData)->dragDataGetCB(pWidget, pDragContex, pSelectionData, uiInfo, uiT);
	}
	static void new_scenario_cb(::GtkMenuItem* pMenuItem, gpointer pUserData)
	{
		// cout<<"new_scenario_cb"<<endl;
		static_cast<CApplication*>(pUserData)->newScenarioCB(pMenuItem);
	}
	static void open_scenario_cb(::GtkMenuItem* pMenuItem, gpointer pUserData)
	{
		// cout<<"open_scenario_cb"<<endl;
		static_cast<CApplication*>(pUserData)->openScenarioCB(pMenuItem);
	}
	static void save_scenario_cb(::GtkMenuItem* pMenuItem, gpointer pUserData)
	{
		// cout<<"save_scenario_cb"<<endl;
		static_cast<CApplication*>(pUserData)->saveScenarioCB(pMenuItem);
	}
	static void save_scenario_as_cb(::GtkMenuItem* pMenuItem, gpointer pUserData)
	{
		// cout<<"save_scenario_as_cb"<<endl;
		static_cast<CApplication*>(pUserData)->saveScenarioAsCB(pMenuItem);
	}
	static void close_scenario_cb(::GtkMenuItem* pMenuItem, gpointer pUserData)
	{
		// cout<<"close_scenario_cb"<<endl;
		static_cast<CApplication*>(pUserData)->closeScenarioCB(pMenuItem);
	}
	static void run_scenario_cb(::GtkButton* pButton, gpointer pUserData)
	{
		cout<<"run_scenario_cb"<<endl;
		static_cast<CApplication*>(pUserData)->runScenarioCB(pButton);
	}
	static void stop_scenario_cb(::GtkButton* pButton, gpointer pUserData)
	{
		cout<<"stop_scenario_cb"<<endl;
		static_cast<CApplication*>(pUserData)->stopScenarioCB(pButton);
	}
	static void pause_scenario_cb(::GtkButton* pButton, gpointer pUserData)
	{
		cout<<"pause_scenario_cb"<<endl;
		static_cast<CApplication*>(pUserData)->pauseScenarioCB(pButton);
	}
	static void step_scenario_cb(::GtkButton* pButton, gpointer pUserData)
	{
		cout<<"step_scenario_cb"<<endl;
		static_cast<CApplication*>(pUserData)->stepScenarioCB(pButton);
	}
	static gboolean idle_scenario_step(gpointer pUserData)
	{
		static_cast<IPlayer*>(pUserData)->loop();
		return TRUE;
	}

public:

	IKernel* m_pKernel;
	IPluginManager* m_pPluginManager;
	IScenarioManager* m_pScenarioManager;

	::GladeXML* m_pGladeInterface;
	::GtkWidget* m_pMainWindow;
	::GtkNotebook* m_pScenarioNotebook;
	::GtkNotebook* m_pBoxAlgorithmNotebook;
	::GtkTreeStore* m_pBoxAlgorithmTreeModel;
	::GtkTreeView* m_pBoxAlgorithmTreeView;

	vector<CInterfacedScenario*> m_vInterfacedScenario;
};

// ------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------

class CPluginDescEnumCB : virtual public Kernel::IPluginManager::IPluginObjectDescEnum
{
public:
	::GtkTreeStore* m_pTreeStore;

	CPluginDescEnumCB(::GtkTreeStore* pTreeStore) : m_pTreeStore(pTreeStore) { }

	virtual boolean callback(
		const Kernel::IPluginModule& rPluginModule,
		const Plugins::IPluginObjectDesc& rPluginObjectDesc)
	{
		// Outputs plugin info to console
		CString l_sModuleFileName;
		rPluginModule.getFileName(l_sModuleFileName);
#if 0
		cout << "   - Plugin                     " << rPluginObjectDesc.getCreatedClass().toString() << endl;
		cout << "       Plugin module filename : " << l_sModuleFileName << endl;
		cout << "       Plugin category        : " << rPluginObjectDesc.getCategory() << endl;
		cout << "       Plugin name            : " << rPluginObjectDesc.getName() << endl;
		cout << "       Author name            : " << rPluginObjectDesc.getAuthorName() << endl;
		cout << "       Author company name    : " << rPluginObjectDesc.getAuthorCompanyName() << endl;
		cout << "       Short description      : " << rPluginObjectDesc.getShortDescription() << endl;
		cout << "       Detailed description   : " << rPluginObjectDesc.getDetailedDescription() << endl;
#endif

		if(rPluginObjectDesc.isDerivedFromClass(OV_ClassId_Plugins_BoxAlgorithmDesc))
		{
			// Splits the plugin category
			vector<string> l_vCategory;
			string l_sCategory=string(rPluginObjectDesc.getCategory());
			size_t j, i=(size_t)-1;
			while((j=l_sCategory.find('/', i+1))!=-1)
			{
				string l_sSubCategory=string(l_sCategory, i+1, j-i-1);
				if(l_sSubCategory!=string(""))
				{
					l_vCategory.push_back(l_sSubCategory);
				}
				i=j;
			}
			if(i+1!=l_sCategory.length())
			{
				l_vCategory.push_back(string(l_sCategory, i+1, l_sCategory.length()-i-1));
			}

			// Fills plugin in the tree
			vector<string>::iterator it;
			::GtkTreeIter l_oGtkIter1;
			::GtkTreeIter l_oGtkIter2;
			::GtkTreeIter* l_pGtkIterParent=NULL;
			::GtkTreeIter* l_pGtkIterChild=&l_oGtkIter1;
			for(it=l_vCategory.begin(); it!=l_vCategory.end(); it++)
			{
				boolean l_bFound=false;
				boolean l_bValid=gtk_tree_model_iter_children(
					GTK_TREE_MODEL(m_pTreeStore),
					l_pGtkIterChild,
					l_pGtkIterParent)?true:false;
				while(l_bValid && !l_bFound)
				{
					gchar* l_sName=NULL;
					gboolean l_bIsPlugin;
					gtk_tree_model_get(
						GTK_TREE_MODEL(m_pTreeStore),
						l_pGtkIterChild,
						BoxAlgorithm_StringName, &l_sName,
						BoxAlgorithm_BooleanIsPlugin, &l_bIsPlugin,
						-1);
					if(!l_bIsPlugin && l_sName==*it)
					{
						l_bFound=true;
					}
					else
					{
						l_bValid=gtk_tree_model_iter_next(
							GTK_TREE_MODEL(m_pTreeStore),
							l_pGtkIterChild)?true:false;
					}
				}
				if(!l_bFound)
				{
					gtk_tree_store_append(
						m_pTreeStore,
						l_pGtkIterChild,
						l_pGtkIterParent);
					gtk_tree_store_set(
						m_pTreeStore,
						l_pGtkIterChild,
						BoxAlgorithm_StringName, it->c_str(),
						BoxAlgorithm_StringShortDescription, "",
						BoxAlgorithm_StringStockIcon, "gtk-directory",
						BoxAlgorithm_BooleanIsPlugin, false,
						-1);
				}
				if(!l_pGtkIterParent)
				{
					l_pGtkIterParent=&l_oGtkIter2;
				}
				::GtkTreeIter* l_pGtkIterSwap=l_pGtkIterChild;
				l_pGtkIterChild=l_pGtkIterParent;
				l_pGtkIterParent=l_pGtkIterSwap;
			}
			gtk_tree_store_append(
				m_pTreeStore,
				l_pGtkIterChild,
				l_pGtkIterParent);
			gtk_tree_store_set(
				m_pTreeStore,
				l_pGtkIterChild,
				BoxAlgorithm_StringName, (const char*)rPluginObjectDesc.getName(),
				BoxAlgorithm_StringShortDescription, (const char*)rPluginObjectDesc.getShortDescription(),
				BoxAlgorithm_StringIdentifier, (const char*)rPluginObjectDesc.getCreatedClass().toString(),
				BoxAlgorithm_StringStockIcon, "gtk-execute",
				BoxAlgorithm_BooleanIsPlugin, true,
				-1);
		}

		return true;
	}
};

// ------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------

char** g_argv;
int g_argc;

// ------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------

int main(int argc, char ** argv)
{
	g_argc=argc;
	g_argv=argv;

	::GdkColor l_oColor[]=
	{
		{ 0,     0,     0,     0 },
		{ 0, 16383, 16383, 16383 },
		{ 0, 32767, 32767, 32767 },
		{ 0, 49151, 49151, 49151 },
		{ 0, 65535, 65535, 65535 },
	};

	#define gdk_color_set(c, r, g, b) { c.pixel=0; c.red=r; c.green=g; c.blue=b; }
	gdk_color_set(g_vColors[Color_BoxBackgroundSelected], 65535, 65535, 49151);
	gdk_color_set(g_vColors[Color_BoxBackground],         65535, 65535, 65535);
	gdk_color_set(g_vColors[Color_BoxBorderSelected],         0,     0,     0);
	gdk_color_set(g_vColors[Color_BoxBorder],                 0,     0,     0);
	gdk_color_set(g_vColors[Color_BoxInputBackground],    65535, 49151, 32767);
	gdk_color_set(g_vColors[Color_BoxInputBorder],        32767, 16383,     0);
	gdk_color_set(g_vColors[Color_BoxOutputBackground],   32767, 65535, 49151);
	gdk_color_set(g_vColors[Color_BoxOutputBorder],           0, 32767, 16383);
	gdk_color_set(g_vColors[Color_BoxSettingBackground],  49151, 32767, 65535);
	gdk_color_set(g_vColors[Color_BoxSettingBorder],      16383,     0, 32767);
	gdk_color_set(g_vColors[Color_Link],                      0,     0,     0);
	gdk_color_set(g_vColors[Color_LinkSelected],          65535, 65535, 49151);
	#undef gdk_color_set

//___________________________________________________________________//
//                                                                   //

	IKernelLoader* l_pKernelLoader=OpenViBE::Kernel::createKernelLoader();
	if(l_pKernelLoader)
	{
		cout<<"[  OK  ] Created kernel loader"<<endl;
		CString m_sError;
#ifdef WIN32
		if(!l_pKernelLoader->load("../lib/OpenViBE-Kernel-dynamic.dll", &m_sError))
#else
		if(!l_pKernelLoader->load("../lib/libOpenViBE-Kernel-dynamic.so", &m_sError))
#endif
		{
				cout<<"[FAILED] Error loading kernel ("<<m_sError<<")"<<endl;
		}
		else
		{
			cout<<"[  OK  ] Kernel module loaded"<<endl;
			IKernelDesc* l_pKernelDesc=NULL;
			IKernel* l_pKernel=NULL;
			l_pKernelLoader->initialize();
			l_pKernelLoader->getKernelDesc(l_pKernelDesc);
			if(!l_pKernelDesc)
			{
				cout<<"[FAILED] No kernel descriptor"<<endl;
			}
			else
			{
				cout<<"[  OK  ] Found kernel descriptor"<<endl;
				l_pKernel=l_pKernelDesc->create();
				if(!l_pKernel)
				{
					cout<<"[FAILED] No kernel created by kernel descriptor"<<endl;
				}
				else
				{
					cout<<"[  OK  ] Created Kernel, going on testing"<<endl;

					gtk_init(&g_argc, &g_argv);
					::CApplication app(l_pKernel);
					app.init();

					IPluginManager& l_rPluginManager=l_pKernel->getContext()->getPluginManager();
					l_rPluginManager.addPluginsFromFiles("../lib/libOpenViBE-Plugins-*.so");
					l_rPluginManager.addPluginsFromFiles("../lib/libOpenViBE-*.so");
					l_rPluginManager.addPluginsFromFiles("../lib/OpenViBE-Plugins-*.dll");
					l_rPluginManager.addPluginsFromFiles("../lib/OpenViBE-*.dll");

					CPluginDescEnumCB cb(app.m_pBoxAlgorithmTreeModel);
					l_rPluginManager.enumeratePluginObjectDesc(cb, OV_ClassId_Plugins_BoxAlgorithmDesc);
					l_rPluginManager.enumeratePluginObjectDesc(cb, OV_ClassId_Plugins_ScenarioExporterDesc);
					l_rPluginManager.enumeratePluginObjectDesc(cb, OV_ClassId_Plugins_ScenarioImporterDesc);

					gtk_main();

					cout<<"[  OK  ] Everything finished, realeasing objects"<<endl;
					l_pKernel->release();
				}
				l_pKernelDesc->release();
			}
			l_pKernelLoader->uninitialize();
			l_pKernelLoader->unload();
		}
		l_pKernelLoader->release();
	}

	return 0;
}

#else

#include <iostream>
#include <math.h>

int main(int argc, char ** argv)
{
	std::cout<<"hello world"<<std::endl;

	srand(time(NULL));
	for(int i=0; i<15; i++)
	{
		printf("#define OV_ClassId_                                        OpenViBE::CIdentifier(0x%08X, 0x%08X)\n", rand(), rand());
	}

	return 0;
}

#endif
