//
// PivotPage.xaml.cpp
// Implementation of the PivotPage class.
//

#include "pch.h"
#include "PivotPage.xaml.h"
#include "ItemPage.xaml.h"

using namespace wp81HCIWiimote;
using namespace wp81HCIWiimote::Common;
using namespace wp81HCIWiimote::Data;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::Resources;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Pivot Application template is documented at http://go.microsoft.com/fwlink/?LinkID=391641

PivotPage::PivotPage()
{
	InitializeComponent();

	NavigationCacheMode = Navigation::NavigationCacheMode::Required;

	_resourceLoader = ResourceLoader::GetForCurrentView(L"Resources");

	auto navigationHelper = ref new Common::NavigationHelper(this);
	navigationHelper->LoadState += ref new LoadStateEventHandler(this, &PivotPage::NavigationHelper_LoadState);
	navigationHelper->SaveState += ref new SaveStateEventHandler(this, &PivotPage::NavigationHelper_SaveState);

	SetValue(_defaultViewModelProperty, ref new Platform::Collections::Map<String^, Object^>(std::less<String^>()));
	SetValue(_navigationHelperProperty, navigationHelper);
}

DependencyProperty^ PivotPage::_navigationHelperProperty = nullptr;
DependencyProperty^ PivotPage::_defaultViewModelProperty = nullptr;

void PivotPage::RegisterDependencyProperties()
{
	if (_navigationHelperProperty == nullptr)
	{
		_navigationHelperProperty = DependencyProperty::Register("NavigationHelper",
			TypeName(Common::NavigationHelper::typeid), TypeName(PivotPage::typeid), nullptr);
	}

	if (_defaultViewModelProperty == nullptr)
	{
		_defaultViewModelProperty =
			DependencyProperty::Register("DefaultViewModel",
			TypeName(IObservableMap<String^, Object^>::typeid), TypeName(PivotPage::typeid), nullptr);
	}
}


IObservableMap<String^, Object^>^ PivotPage::DefaultViewModel::get()
{
	return safe_cast<IObservableMap<String^, Object^>^>(GetValue(_defaultViewModelProperty));
}

NavigationHelper^ PivotPage::NavigationHelper::get()
{
	return safe_cast<Common::NavigationHelper^>(GetValue(_navigationHelperProperty));
}

void PivotPage::OnNavigatedTo(NavigationEventArgs^ e)
{
	NavigationHelper->OnNavigatedTo(e);
}

void PivotPage::OnNavigatedFrom(NavigationEventArgs^ e)
{
	NavigationHelper->OnNavigatedFrom(e);
}

/// <summary>
/// Populates the page with content passed during navigation. Any saved state is also
/// provided when recreating a page from a prior session.
/// </summary>
/// <param name="sender">
/// The source of the event; typically <see cref="NavigationHelper"/>.
/// </param>
/// <param name="e">Event data that provides both the navigation parameter passed to
/// <see cref="Frame->Navigate(Type, Object)"/> when this page was initially requested and
/// a dictionary of state preserved by this page during an earlier
/// session. The state will be null the first time a page is visited.</param>
void PivotPage::NavigationHelper_LoadState(Object^ sender, LoadStateEventArgs^ e)
{
	(void) sender;	// Unused parameter
	(void) e;		// Unused parameter

	SampleDataSource::GetGroup(L"Group-1").then([this](SampleDataGroup^ sampleDataGroup)
	{
		DefaultViewModel->Insert(GetFirstGroupName(), sampleDataGroup);
	}, task_continuation_context::use_current());
}

/// <summary>
/// Preserves state associated with this page in case the application is suspended or the
/// page is discarded from the navigation cache. Values must conform to the serialization
/// requirements of <see cref="SuspensionManager.SessionState"/>.
/// </summary>
/// <param name="sender">
/// The source of the event; typically <see cref="NavigationHelper"/>.
/// </param>
/// <param name="e">
/// Event data that provides an empty dictionary to be populated with serializable state.
/// </param>
void PivotPage::NavigationHelper_SaveState(Object^ sender, SaveStateEventArgs^ e)
{
	// TODO: Save the unique state of the page here.
}

/// <summary>
/// Loads the content for the second pivot item when it is scrolled into view.
/// </summary>
void PivotPage::SecondPivot_Loaded(Object^ sender, RoutedEventArgs ^e)
{
	(void) sender;	// Unused parameter
	(void) e;		// Unused parameter

	// TODO: Create an appropriate data model for your problem domain to replace the sample data.
	SampleDataSource::GetGroup(L"Group-2").then([this](SampleDataGroup^ sampleDataGroup)
	{
		DefaultViewModel->Insert(GetSecondGroupName(), sampleDataGroup);
	}, task_continuation_context::use_current());
}

/// <summary>
/// Adds an item to the list when the app bar button is clicked.
/// </summary>
void PivotPage::AddAppBarButton_Click(Object^ sender, RoutedEventArgs ^e)
{
	String^ groupName;
	if (pivot->SelectedIndex == 0)
	{
		groupName = GetFirstGroupName();
	}
	else
	{
		groupName = GetSecondGroupName();
	}

	auto group = safe_cast<SampleDataGroup^>(DefaultViewModel->Lookup(groupName));
	auto nextItemId = group->Items->Size + 1;
	std::wstring uniqueId = std::wstring(L"Group-") + std::to_wstring(pivot->SelectedIndex + 1) + std::wstring(L"-Item-") + std::to_wstring(nextItemId);
	std::wstring title = std::wstring(L"Item Title: ") + std::to_wstring(nextItemId);

	auto newItem = ref new SampleDataItem(
		ref new String(uniqueId.c_str()),
		ref new String(title.c_str()),
		nullptr,
		nullptr,
		_resourceLoader->GetString("NewItemDescription"),
		nullptr);

	group->Items->Append(newItem);

	// Scroll the new item into view.
	auto container = safe_cast<ContentControl^>(pivot->ContainerFromIndex(pivot->SelectedIndex));
	auto listView = safe_cast<ListView^>(container->ContentTemplateRoot);
	listView->ScrollIntoView(newItem, ScrollIntoViewAlignment::Leading);
}

/// <summary>
/// Invoked when an item within a section is clicked.
/// </summary>
void PivotPage::ItemView_ItemClick(Object^ sender, ItemClickEventArgs ^e)
{
	auto itemId = safe_cast<SampleDataItem^>(e->ClickedItem)->UniqueId;
	if (!Frame->Navigate(ItemPage::typeid, itemId))
	{
		throw ref new FailureException(_resourceLoader->GetString(L"NavigationFailedExceptionMessage"));
	}
}
