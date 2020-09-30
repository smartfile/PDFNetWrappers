//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/Annot.h>
#include <PDF/Field.h>
#include <PDF/Font.h>
#include <PDF/ElementBuilder.h>
#include <PDF/ElementWriter.h>
#include <PDF/Annots/Widget.h>
#include <PDF/ViewChangeCollection.h>
#include <PDF/Annots/TextWidget.h>
#include <PDF/Annots/CheckBoxWidget.h>
#include <PDF/Annots/ComboBoxWidget.h>
#include <PDF/Annots/ListBoxWidget.h>
#include <PDF/Annots/PushButtonWidget.h>
#include <PDF/Annots/RadioButtonWidget.h>
#include <PDF/Annots/RadioButtonGroup.h>
#include <PDF/Annots/SignatureWidget.h>
#include <iostream>
#include <map>
#include <cmath>

using namespace std;
using namespace pdftron;
using namespace SDF;
using namespace PDF;
using namespace Annots;
//---------------------------------------------------------------------------------------
// This sample illustrates basic PDFNet capabilities related to interactive 
// forms (also known as AcroForms). 
//---------------------------------------------------------------------------------------

// field_nums has to be greater than 0.
void RenameAllFields(PDFDoc& doc, const UString& name, int field_nums = 1)
{
	char tmp[32];
	FieldIterator itr = doc.GetFieldIterator(name);
	for (int counter = 1; itr.HasNext();
		itr = doc.GetFieldIterator(name), ++counter)
	{
		Field f = itr.Current();
		int update_count = int(ceil(counter/(double)field_nums));
		sprintf(tmp, "-%d", update_count);
		f.Rename(name + tmp);	
	}
}

Obj CreateCustomButtonAppearance(PDFDoc& doc, bool button_down) 
{
	// Create a button appearance stream ------------------------------------
	ElementBuilder build;
	ElementWriter writer;
	writer.Begin(doc); 

	// Draw background
	Element element = build.CreateRect(0, 0, 101, 37);
	element.SetPathFill(true);
	element.SetPathStroke(false);
	element.GetGState().SetFillColorSpace(ColorSpace::CreateDeviceGray());
	element.GetGState().SetFillColor(ColorPt(0.75));
	writer.WriteElement(element); 

	// Draw 'Submit' text
	writer.WriteElement(build.CreateTextBegin()); 
	{
		const char* text = "Submit";
		element = build.CreateTextRun(text, UInt32(strlen(text)), Font::Create(doc, PDF::Font::e_helvetica_bold), 12);
		element.GetGState().SetFillColor(ColorPt(0));

		if (button_down) 
			element.SetTextMatrix(1, 0, 0, 1, 33, 10);
		else 
			element.SetTextMatrix(1, 0, 0, 1, 30, 13);
		writer.WriteElement(element);
	}
	writer.WriteElement(build.CreateTextEnd());

	Obj stm = writer.End(); 

	// Set the bounding box
	stm.PutRect("BBox", 0, 0, 101, 37);
	stm.PutName("Subtype","Form");
	return stm;
}

int main(int argc, char *argv[])
{
	int ret = 0;
	PDFNet::Initialize();

	// Relative path to the folder containing test files.
	// string input_path =  "../../TestFiles/";
	string output_path = "../../TestFiles/Output/";

	// The vector used to store the name and count of all fields.
	// This is used later on to clone the fields
	map<UString, int> field_names;

	//----------------------------------------------------------------------------------
	// Example 1: Programatically create new Form Fields and Widget Annotations.
	//----------------------------------------------------------------------------------
	try  
	{
		PDFDoc doc;
		// Create a blank new page and add some form fields.
		Page blank_page = doc.PageCreate();

		// Text Widget Creation 
		// Create an empty text widget with black text.
		TextWidget text1 = TextWidget::Create(doc, Rect(110, 700, 380, 730));
		text1.SetText(UString("Basic Text Field"));
		text1.RefreshAppearance();
		blank_page.AnnotPushBack(text1);
		// Create a vertical text widget with blue text and a yellow background.
		TextWidget text2 = TextWidget::Create(doc, Rect(50, 400, 90, 730));
		text2.SetRotation(90);
		// Set the text content.
		text2.SetText(UString("    ****Lucky Stars!****"));
		// Set the font type, text color, font size, border color and background color.
		text2.SetFont(Font::Create(doc, Font::e_helvetica_oblique));
		text2.SetFontSize(28);
		text2.SetTextColor(ColorPt(0, 0, 1), 3);
		text2.SetBorderColor(ColorPt(0, 0, 0), 3);
		text2.SetBackgroundColor(ColorPt(1, 1, 0), 3);
		text2.RefreshAppearance();
		// Add the annotation to the page.
		blank_page.AnnotPushBack(text2);
		// Create two new text widget with Field names employee.name.first and employee.name.last
		// This logic shows how these widgets can be created using either a field name string or
		// a Field object
		TextWidget text3 = TextWidget::Create(doc, Rect(110, 660, 380, 690), "employee.name.first");
		text3.SetText(UString("Levi"));
		text3.SetFont(Font::Create(doc, Font::e_times_bold));
		text3.RefreshAppearance();
		blank_page.AnnotPushBack(text3);
		Field emp_last_name = doc.FieldCreate("employee.name.last", Field::e_text, "Ackerman");
		TextWidget text4 = TextWidget::Create(doc, Rect(110, 620, 380, 650), emp_last_name);
		text4.SetFont(Font::Create(doc, Font::e_times_bold));
		text4.RefreshAppearance();
		blank_page.AnnotPushBack(text4);

		// Signature Widget Creation (unsigned)
		SignatureWidget signature1 = SignatureWidget::Create(doc, Rect(110, 560, 260, 610));
		signature1.RefreshAppearance();
		blank_page.AnnotPushBack(signature1);

		// CheckBox Widget Creation
		// Create a check box widget that is not checked.
		CheckBoxWidget check1 = CheckBoxWidget::Create(doc, Rect(140, 490, 170, 520));
		check1.RefreshAppearance();
		blank_page.AnnotPushBack(check1);
		// Create a check box widget that is checked.
		CheckBoxWidget check2 = CheckBoxWidget::Create(doc, Rect(190, 490, 250, 540), "employee.name.check1");
		check2.SetBackgroundColor(ColorPt(1, 1, 1), 3);
		check2.SetBorderColor(ColorPt(0, 0, 0), 3);
		// Check the widget (by default it is unchecked).
		check2.SetChecked(true);
		check2.RefreshAppearance();
		blank_page.AnnotPushBack(check2);

		// PushButton Widget Creation
		PushButtonWidget pushbutton1 = PushButtonWidget::Create(doc, Rect(380, 490, 520, 540));
		pushbutton1.SetTextColor(ColorPt(1, 1, 1), 3);
		pushbutton1.SetFontSize(36);
		pushbutton1.SetBackgroundColor(ColorPt(0, 0, 0), 3);
		// Add a caption for the pushbutton.
		pushbutton1.SetStaticCaptionText("PushButton");
		pushbutton1.RefreshAppearance();
		blank_page.AnnotPushBack(pushbutton1);

		// ComboBox Widget Creation
		ComboBoxWidget combo1 = ComboBoxWidget::Create(doc, Rect(280, 560, 580, 610));
		// Add options to the combobox widget.
		combo1.AddOption("Combo Box No.1");
		combo1.AddOption("Combo Box No.2");
		combo1.AddOption("Combo Box No.3");
		// Make one of the options in the combo box selected by default.
		combo1.SetSelectedOption(UString("Combo Box No.2"));
		combo1.SetTextColor(ColorPt(1, 0, 0), 3);
		combo1.SetFontSize(28);
		combo1.RefreshAppearance();
		blank_page.AnnotPushBack(combo1);

		// ListBox Widget Creation
		ListBoxWidget list1 = ListBoxWidget::Create(doc, Rect(400, 620, 580, 730));
		// Add one option to the listbox widget.
		list1.AddOption("List Box No.1");
		// Add multiple options to the listbox widget in a batch.
		vector<UString> list_options;
		list_options.push_back("List Box No.2");
		list_options.push_back("List Box No.3");
		list1.AddOptions(list_options);
		// Select some of the options in list box as default options
		list1.SetSelectedOptions(list_options);
		// Enable list box to have multi-select when editing. 
		list1.GetField().SetFlag(Field::e_multiselect, true);
		list1.SetFont(Font::Create(doc, Font::e_times_italic));
		list1.SetTextColor(ColorPt(1, 0, 0), 3);
		list1.SetFontSize(28);
		list1.SetBackgroundColor(ColorPt(1, 1, 1), 3);
		list1.RefreshAppearance();
		blank_page.AnnotPushBack(list1);

		// RadioButton Widget Creation
		// Create a radio button group and add three radio buttons in it. 
		RadioButtonGroup radio_group = RadioButtonGroup::Create(doc, "RadioGroup");
		RadioButtonWidget radiobutton1 = radio_group.Add(Rect(140, 410, 190, 460));
		radiobutton1.SetBackgroundColor(ColorPt(1, 1, 0), 3);
		radiobutton1.RefreshAppearance();
		RadioButtonWidget radiobutton2 = radio_group.Add(Rect(310, 410, 360, 460));
		radiobutton2.SetBackgroundColor(ColorPt(0, 1, 0), 3);
		radiobutton2.RefreshAppearance();
		RadioButtonWidget radiobutton3 = radio_group.Add(Rect(480, 410, 530, 460));
		// Enable the third radio button. By default the first one is selected
		radiobutton3.EnableButton();
		radiobutton3.SetBackgroundColor(ColorPt(0, 1, 1), 3);
		radiobutton3.RefreshAppearance();
		radio_group.AddGroupButtonsToPage(blank_page);

		// Custom push button annotation creation
		PushButtonWidget custom_pushbutton1 = PushButtonWidget::Create(doc, Rect(260, 320, 360, 360));
		// Set the annotation appearance.
		custom_pushbutton1.SetAppearance(CreateCustomButtonAppearance(doc, false), Annot::e_normal);
		// Create 'SubmitForm' action. The action will be linked to the button.
		FileSpec url = FileSpec::CreateURL(doc, "http://www.pdftron.com");
		Action button_action = Action::CreateSubmitForm(url);
		// Associate the above action with 'Down' event in annotations action dictionary.
		Obj annot_action = custom_pushbutton1.GetSDFObj().PutDict("AA");
		annot_action.Put("D", button_action.GetSDFObj());
		blank_page.AnnotPushBack(custom_pushbutton1);

		// Add the page as the last page in the document.
		doc.PagePushBack(blank_page);	
		// If you are not satisfied with the look of default auto-generated appearance 
		// streams you can delete "AP" entry from the Widget annotation and set 
		// "NeedAppearances" flag in AcroForm dictionary:
		//    doc.GetAcroForm().PutBool("NeedAppearances", true);
		// This will force the viewer application to auto-generate new appearance streams 
		// every time the document is opened.
		//
		// Alternatively you can generate custom annotation appearance using ElementWriter 
		// and then set the "AP" entry in the widget dictionary to the new appearance
		// stream.
		//
		// Yet another option is to pre-populate field entries with dummy text. When 
		// you edit the field values using PDFNet the new field appearances will match 
		// the old ones.

		//doc.GetAcroForm().PutBool("NeedAppearances", true);
		// NOTE: RefreshFieldAppearances will replace previously generated appearance streams
		doc.RefreshFieldAppearances();

		doc.Save((output_path + "forms_test1.pdf").c_str(), 0, 0);
		cout << "Done." << endl;
	}
	catch(Common::Exception& e)
	{
		cout << e << endl;
		ret = 1;
	}
	catch(...)
	{
		cout << "Unknown Exception" << endl;
		ret = 1;
	}

	//----------------------------------------------------------------------------------
	// Example 2: 
	// Fill-in forms / Modify values of existing fields.
	// Traverse all form fields in the document (and print out their names). 
	// Search for specific fields in the document.
	//----------------------------------------------------------------------------------
	try  
	{
		PDFDoc doc((output_path + "forms_test1.pdf").c_str());
		doc.InitSecurityHandler();

		FieldIterator itr = doc.GetFieldIterator();
		for(; itr.HasNext(); itr.Next()) 
		{
			UString cur_field_name = itr.Current().GetName();

			// Add one to the count for this field name for later processing
			field_names[cur_field_name] = (field_names.count(cur_field_name) ? field_names[cur_field_name] + 1 : 1);

			cout << "Field name: " << itr.Current().GetName() << endl;
			cout << "Field partial name: " << itr.Current().GetPartialName() << endl;
			cout << "Field type: ";
			Field::Type type = itr.Current().GetType();
			UString str_val = itr.Current().GetValueAsString();

			switch(type)
			{
			case Field::e_button: 
				cout << "Button" << endl; 
				break;
			case Field::e_radio: 
				cout << "Radio button: Value = " << str_val << endl; 
				break;
			case Field::e_check: 
				itr.Current().SetValue(true);
				cout << "Check box: Value = " << str_val << endl; 
				break;
			case Field::e_text: 
				{
					cout << "Text" << endl;
					// Edit all variable text in the document
					itr.Current().SetValue(UString("This is a new value. The old one was: ") + str_val);
				}
				break;
			case Field::e_choice: cout << "Choice" << endl; break;
			case Field::e_signature: cout << "Signature" << endl; break;
			}

			cout << "------------------------------" << endl;
		}

		// Search for a specific field
		Field f = doc.GetField("employee.name.first");
		if (f) 
		{
			cout << "Field search for " << f.GetName() << " was successful" << endl;
		}
		else 
		{
			cout << "Field search failed" << endl;
		}

		// Regenerate field appearances.
		doc.RefreshFieldAppearances();
		doc.Save((output_path + "forms_test_edit.pdf").c_str(), 0, NULL);
		cout << "Done." << endl;
	}
	catch(Common::Exception& e)
	{
		cout << e << endl;
		ret = 1;
	}
	catch(...)
	{
		cout << "Unknown Exception" << endl;
		ret = 1;
	}

	//----------------------------------------------------------------------------------
	// Sample: Form templating
	// Replicate pages and form data within a document. Then rename field names to make 
	// them unique.
	//----------------------------------------------------------------------------------
	try  
	{
		// Sample: Copying the page with forms within the same document
		PDFDoc doc((output_path + "forms_test1.pdf").c_str());
		doc.InitSecurityHandler();

		Page src_page = doc.GetPage(1);
		doc.PagePushBack(src_page);  // Append several copies of the first page
		doc.PagePushBack(src_page);	 // Note that forms are successfully copied
		doc.PagePushBack(src_page);
		doc.PagePushBack(src_page);

		// Now we rename fields in order to make every field unique.
		// You can use this technique for dynamic template filling where you have a 'master'
		// form page that should be replicated, but with unique field names on every page. 
		for (map<UString, int>::iterator i = field_names.begin(); i != field_names.end(); i++)
		{
			RenameAllFields(doc, i->first, i->second);
		}

		doc.Save((output_path + "forms_test1_cloned.pdf").c_str(), 0, 0);
		cout << "Done." << endl;
	}
	catch(Common::Exception& e)
	{
		cout << e << endl;
		ret = 1;
	}
	catch(...)
	{
		cout << "Unknown Exception" << endl;
		ret = 1;
	}

	//----------------------------------------------------------------------------------
	// Sample: 
	// Flatten all form fields in a document.
	// Note that this sample is intended to show that it is possible to flatten
	// individual fields. PDFNet provides a utility function PDFDoc.FlattenAnnotations()
	// that will automatically flatten all fields.
	//----------------------------------------------------------------------------------
	try  
	{
		PDFDoc doc((output_path + "forms_test1.pdf").c_str());
		doc.InitSecurityHandler();

		// Traverse all pages
		if (true) {
			doc.FlattenAnnotations();
		}
		else // Manual flattening
		{			
			for (PageIterator pitr = doc.GetPageIterator(); 
				pitr.HasNext(); pitr.Next())  
			{
				Page page = pitr.Current();
				for (Int32 i = ((Int32)page.GetNumAnnots()) - 1; i >= 0; --i)
				{
					Annot annot = page.GetAnnot(i);
					if (annot.GetType() == Annot::e_Widget)
					{
						annot.Flatten(page);
					}
				}
			}
		}

		doc.Save((output_path + "forms_test1_flattened.pdf").c_str(), 0, 0);
		cout << "Done." << endl;
	}
	catch(Common::Exception& e)
	{
		cout << e << endl;
		ret = 1;
	}
	catch(...)
	{
		cout << "Unknown Exception" << endl;
		ret = 1;
	}

	PDFNet::Terminate();
	return ret;
}


