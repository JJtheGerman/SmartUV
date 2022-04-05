#include "Editor/SmartUV_AssetEditorToolkit.h"

#include "Editor.h"
#include "SEditorViewport.h"
#include "EditorReimportHandler.h"
#include "EditorStyleSet.h"
#include "UObject/NameTypes.h"
#include "Widgets/Docking/SDockTab.h"
#include "SCommonEditorViewportToolbarBase.h"
#include "SSingleObjectDetailsPanel.h"

#include "Asset/SmartUV_Asset.h"
#include "Editor/SSmartUV_AssetEditor.h"
#include "Editor/SmartUV_EditorViewportClient.h"

#define LOCTEXT_NAMESPACE "FSmartUV_AssetEditorToolkit"

/////////////////////////////////////////////////////////////

const FName AppIdentifier = FName(TEXT("SmartUV_AssetEditor"));
const FName TabId = FName(TEXT("SmartUV_Editor"));

/////////////////////////////////////////////////////////////

struct FSmartUV_EditorTabs
{
	// Tab identifiers
	static const FName ViewportID;
	static const FName DetailsID;
};

const FName FSmartUV_EditorTabs::ViewportID(TEXT("Viewport"));
const FName FSmartUV_EditorTabs::DetailsID(TEXT("Details"));

/////////////////////////////////////////////////////////////

class SSmartUV_EditorViewport : public SEditorViewport
{
public:
	SLATE_BEGIN_ARGS(SSmartUV_EditorViewport) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedPtr<FSmartUV_AssetEditorToolkit> InSmartUVEditor)
	{
		SmartUV_EditorPtr = InSmartUVEditor;
		SEditorViewport::Construct(SEditorViewport::FArguments());
	}

	// SEditorViewport interface
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override
	{
		EditorViewportClient = MakeShareable(new FSmartUV_EditorViewportClient());
		return EditorViewportClient.ToSharedRef();
	}

	virtual EVisibility GetTransformToolbarVisibility() const override
	{
		return EVisibility::Visible;
	}
	// End of SEditorViewport interface

	ESmartUVEditorMode::Type GetCurrentMode() const
	{
		return EditorViewportClient->GetCurrentMode();
	}

	TSharedPtr<FSmartUV_EditorViewportClient> GetEditorViewportClient() { return EditorViewportClient; };

private:

	// Pointer back to owning SmartUV editor instance
	TWeakPtr<FSmartUV_AssetEditorToolkit> SmartUV_EditorPtr;

	// Viewport client
	TSharedPtr<FSmartUV_EditorViewportClient> EditorViewportClient;
};

/////////////////////////////////////////////////////////////

FSmartUV_AssetEditorToolkit::FSmartUV_AssetEditorToolkit()
{}

FSmartUV_AssetEditorToolkit::~FSmartUV_AssetEditorToolkit()
{
	FReimportManager::Instance()->OnPreReimport().RemoveAll(this);
	FReimportManager::Instance()->OnPostReimport().RemoveAll(this);

	GEditor->UnregisterForUndo(this);
}

/* FSmartUV_AssetEditorToolkit interface
 *****************************************************************************/

void FSmartUV_AssetEditorToolkit::Initialize(USmartUV_Asset* InSmartMaterialAsset, const EToolkitMode::Type InMode, const TSharedPtr<IToolkitHost>& InToolkitHost)
{
	SmartUV_Asset = InSmartMaterialAsset;

	// Init viewport
	ViewportPtr = SNew(SSmartUV_EditorViewport, SharedThis(this));

	// create default tab layout
	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("Standalone_SmartUVAssetEditor_Layout")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Horizontal)
				->SetSizeCoefficient(0.9f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.8f)
					->SetHideTabWell(true)
					->AddTab(FSmartUV_EditorTabs::ViewportID, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewSplitter()
					->SetOrientation(Orient_Vertical)
					->SetSizeCoefficient(0.2f)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.75f)
						->SetHideTabWell(true)
						->AddTab(FSmartUV_EditorTabs::DetailsID, ETabState::OpenedTab)
					)
				)
			)
		);

	// Initialize the asset editor
	InitAssetEditor(InMode, 
		InToolkitHost,
		AppIdentifier,
		Layout,
		true,
		true,
		InSmartMaterialAsset);
}

ESmartUVEditorMode::Type FSmartUV_AssetEditorToolkit::GetCurrentMode() const
{
	return ViewportPtr->GetCurrentMode();
}

FString FSmartUV_AssetEditorToolkit::GetDocumentationLink() const
{
	return FString(TEXT("https://google.com/"));
}

void FSmartUV_AssetEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_SmartUVEditor", "SmartUV Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(FSmartUV_EditorTabs::ViewportID, FOnSpawnTab::CreateSP(this, &FSmartUV_AssetEditorToolkit::SpawnTab_Viewport))
		.SetDisplayName(LOCTEXT("ViewportTab", "Viewport"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));

	InTabManager->RegisterTabSpawner(FSmartUV_EditorTabs::DetailsID, FOnSpawnTab::CreateSP(this, &FSmartUV_AssetEditorToolkit::SpawnTab_Details))
		.SetDisplayName(LOCTEXT("DetailsTabLabel", "Details"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FSmartUV_AssetEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
	InTabManager->UnregisterTabSpawner(FSmartUV_EditorTabs::ViewportID);
	InTabManager->UnregisterTabSpawner(FSmartUV_EditorTabs::DetailsID);
}

FText FSmartUV_AssetEditorToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "SmartUV Asset Editor");
}

FName FSmartUV_AssetEditorToolkit::GetToolkitFName() const
{
	return FName("SmartUV_AssetEditor");
}

FLinearColor FSmartUV_AssetEditorToolkit::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.3f, 0.2f, 0.5f, 0.5f);
}

FString FSmartUV_AssetEditorToolkit::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "SmartUV_Asset ").ToString();
}

void FSmartUV_AssetEditorToolkit::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(SmartUV_Asset);
}

TSharedRef<SDockTab> FSmartUV_AssetEditorToolkit::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.Label(LOCTEXT("ViewportTab_Title", "Viewport"))
		[
			SNew(SOverlay)

			// The SmartUV editor viewport
			+SOverlay::Slot()
			[
				ViewportPtr.ToSharedRef()
			]
		];
}

TSharedRef<SDockTab> FSmartUV_AssetEditorToolkit::SpawnTab_Details(const FSpawnTabArgs& Args)
{	
	TSharedPtr<FSmartUV_AssetEditorToolkit> SmartUV_EditorPtr = SharedThis(this);

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	MainPropertyView = PropertyModule.CreateDetailView(FDetailsViewArgs());
	MainPropertyView->SetObject(SmartUV_Asset);

	// Delegate when properties are changed
	MainPropertyView->OnFinishedChangingProperties().AddRaw(this, &FSmartUV_AssetEditorToolkit::OnPropertyChanged);

	// The SmartUV details tab
	return SNew(SDockTab)
		.Label(LOCTEXT("DetailsTab_Title", "Details"))
		[
			MainPropertyView.ToSharedRef()
		];
}

void FSmartUV_AssetEditorToolkit::OnPropertyChanged(const FPropertyChangedEvent& PropertyChangedEvent)
{
	FProperty* Property = PropertyChangedEvent.Property;
	if (Property)
	{
		// Change the editor viewports preview texture
		if (Property->GetFName() == GET_MEMBER_NAME_CHECKED(USmartUV_Asset, PreviewTexture))
		{
			ViewportPtr->GetEditorViewportClient()->UpdatePreviewMaterial(SmartUV_Asset->PreviewTexture);
		}
	}
}

#undef LOCTEXT_NAMESPACE