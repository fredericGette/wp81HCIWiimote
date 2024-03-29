﻿//
// ItemPage.xaml.cpp
// Implementation of the ItemPage class.
//

#include "pch.h"
#include "ItemPage.xaml.h"

using namespace wp81HCIWiimote;
using namespace wp81HCIWiimote::Common;
using namespace wp81HCIWiimote::Data;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
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

ItemPage::ItemPage()
{
	InitializeComponent();

	NavigationCacheMode = Navigation::NavigationCacheMode::Required;

	auto navigationHelper = ref new Common::NavigationHelper(this);
	navigationHelper->LoadState += ref new LoadStateEventHandler(this, &ItemPage::NavigationHelper_LoadState);
	navigationHelper->SaveState += ref new SaveStateEventHandler(this, &ItemPage::NavigationHelper_SaveState);

	SetValue(_defaultViewModelProperty, ref new Platform::Collections::Map<String^, Object^>(std::less<String^>()));
	SetValue(_navigationHelperProperty, navigationHelper);
}

DependencyProperty^ ItemPage::_navigationHelperProperty = nullptr;
DependencyProperty^ ItemPage::_defaultViewModelProperty = nullptr;

void ItemPage::RegisterDependencyProperties()
{
	if (_navigationHelperProperty == nullptr)
	{
		_navigationHelperProperty = DependencyProperty::Register("NavigationHelper",
			TypeName(Common::NavigationHelper::typeid), TypeName(ItemPage::typeid), nullptr);
	}

	if (_defaultViewModelProperty == nullptr)
	{
		_defaultViewModelProperty =
			DependencyProperty::Register("DefaultViewModel",
			TypeName(IObservableMap<String^, Object^>::typeid), TypeName(ItemPage::typeid), nullptr);
	}
}

IObservableMap<String^, Object^>^ ItemPage::DefaultViewModel::get()
{
	return safe_cast<IObservableMap<String^, Object^>^>(GetValue(_defaultViewModelProperty));
}

NavigationHelper^ ItemPage::NavigationHelper::get()
{
	return safe_cast<Common::NavigationHelper^>(GetValue(_navigationHelperProperty));
}

void ItemPage::OnNavigatedTo(NavigationEventArgs^ e)
{
	NavigationHelper->OnNavigatedTo(e);
}

void ItemPage::OnNavigatedFrom(NavigationEventArgs^ e)
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
void ItemPage::NavigationHelper_LoadState(Object^ sender, LoadStateEventArgs^ e)
{
	(void) sender;	// Unused parameter
	(void) e;		// Unused parameter

	// TODO: Create an appropriate data model for your problem domain to replace the sample data.
	SampleDataSource::GetItem(safe_cast<String^>(e->NavigationParameter)).then([this](SampleDataItem^ sampleDataItem)
	{
		this->DefaultViewModel->Insert("Item", sampleDataItem);
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
void ItemPage::NavigationHelper_SaveState(Object^ sender, SaveStateEventArgs^ e)
{
	// TODO: Save the unique state of the page here.
}
