//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

using System;
using System.Collections.Generic;
using pdftron;
using pdftron.Common;
using pdftron.SDF;
using pdftron.PDF;
using pdftron.PDF.Annots;

namespace FormsTestCS
{
	/// <summary>
	///---------------------------------------------------------------------------------------
	/// This sample illustrates basic PDFNet capabilities related to interactive 
	/// forms (also known as AcroForms). 
	///---------------------------------------------------------------------------------------
	/// </summary>
	class Class1
	{
		private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static Class1() {}
		
		static void Main(string[] args)
		{
			PDFNet.Initialize();

			// Relative path to the folder containing test files.
			// string input_path =  "../../TestFiles/";
			string output_path = "../../TestFiles/Output/";

            // The vector used to store the name and count of all fields.
            // This is used later on to clone the fields
            Dictionary<string, int> field_names = new Dictionary<string, int>();

            //----------------------------------------------------------------------------------
            // Example 1: Programatically create new Form Fields and Widget Annotations.
            //----------------------------------------------------------------------------------
            try
			{
				using (PDFDoc doc = new PDFDoc())
				{
                    // Create a blank new page and add some form fields.
                    Page blank_page = doc.PageCreate();

                    // Text Widget Creation 
                    // Create an empty text widget with black text.
                    TextWidget text1 = TextWidget.Create(doc, new Rect(110, 700, 380, 730));
                    text1.SetText("Basic Text Field");
                    text1.RefreshAppearance();
                    blank_page.AnnotPushBack(text1);
                    // Create a vertical text widget with blue text and a yellow background.
                    TextWidget text2 = TextWidget.Create(doc, new Rect(50, 400, 90, 730));
                    text2.SetRotation(90);
                    // Set the text content.
                    text2.SetText("    ****Lucky Stars!****");
                    // Set the font type, text color, font size, border color and background color.
                    text2.SetFont(Font.Create(doc, Font.StandardType1Font.e_helvetica_oblique));
                    text2.SetFontSize(28);
                    text2.SetTextColor(new ColorPt(0, 0, 1), 3);
                    text2.SetBorderColor(new ColorPt(0, 0, 0), 3);
                    text2.SetBackgroundColor(new ColorPt(1, 1, 0), 3);
                    text2.RefreshAppearance();
                    // Add the annotation to the page.
                    blank_page.AnnotPushBack(text2);
                    // Create two new text widget with Field names employee.name.first and employee.name.last
                    // This logic shows how these widgets can be created using either a field name string or
                    // a Field object
                    TextWidget text3 = TextWidget.Create(doc, new Rect(110, 660, 380, 690), "employee.name.first");
                    text3.SetText("Levi");
                    text3.SetFont(Font.Create(doc, Font.StandardType1Font.e_times_bold));
                    text3.RefreshAppearance();
                    blank_page.AnnotPushBack(text3);
                    Field emp_last_name = doc.FieldCreate("employee.name.last", Field.Type.e_text, "Ackerman");
                    TextWidget text4 = TextWidget.Create(doc, new Rect(110, 620, 380, 650), emp_last_name);
                    text4.SetFont(Font.Create(doc, Font.StandardType1Font.e_times_bold));
                    text4.RefreshAppearance();
                    blank_page.AnnotPushBack(text4);

                    // Signature Widget Creation (unsigned)
                    SignatureWidget signature1 = SignatureWidget.Create(doc, new Rect(110, 560, 260, 610));
                    signature1.RefreshAppearance();
                    blank_page.AnnotPushBack(signature1);

                    // CheckBox Widget Creation
                    // Create a check box widget that is not checked.
                    CheckBoxWidget check1 = CheckBoxWidget.Create(doc, new Rect(140, 490, 170, 520));
                    check1.RefreshAppearance();
                    blank_page.AnnotPushBack(check1);
                    // Create a check box widget that is checked.
                    CheckBoxWidget check2 = CheckBoxWidget.Create(doc, new Rect(190, 490, 250, 540), "employee.name.check1");
                    check2.SetBackgroundColor(new ColorPt(1, 1, 1), 3);
                    check2.SetBorderColor(new ColorPt(0, 0, 0), 3);
                    // Check the widget (by default it is unchecked).
                    check2.SetChecked(true);
                    check2.RefreshAppearance();
                    blank_page.AnnotPushBack(check2);

                    // PushButton Widget Creation
                    PushButtonWidget pushbutton1 = PushButtonWidget.Create(doc, new Rect(380, 490, 520, 540));
                    pushbutton1.SetTextColor(new ColorPt(1, 1, 1), 3);
                    pushbutton1.SetFontSize(36);
                    pushbutton1.SetBackgroundColor(new ColorPt(0, 0, 0), 3);
                    // Add a caption for the pushbutton.
                    pushbutton1.SetStaticCaptionText("PushButton");
                    pushbutton1.RefreshAppearance();
                    blank_page.AnnotPushBack(pushbutton1);

                    // ComboBox Widget Creation
                    ComboBoxWidget combo1 = ComboBoxWidget.Create(doc, new Rect(280, 560, 580, 610));
                    // Add options to the combobox widget.
                    combo1.AddOption("Combo Box No.1");
                    combo1.AddOption("Combo Box No.2");
                    combo1.AddOption("Combo Box No.3");
                    // Make one of the options in the combo box selected by default.
                    combo1.SetSelectedOption("Combo Box No.2");
                    combo1.SetTextColor(new ColorPt(1, 0, 0), 3);
                    combo1.SetFontSize(28);
                    combo1.RefreshAppearance();
                    blank_page.AnnotPushBack(combo1);

                    // ListBox Widget Creation
                   ListBoxWidget list1 = ListBoxWidget.Create(doc, new Rect(400, 620, 580, 730));
                    // Add one option to the listbox widget.
                    list1.AddOption("List Box No.1");
                    // Add multiple options to the listbox widget in a batch.
                    string[] list_options = new string[2] {"List Box No.2", "List Box No.3"};
                    list1.AddOptions(list_options);
                    // Select some of the options in list box as default options
                    list1.SetSelectedOptions(list_options);
                    // Enable list box to have multi-select when editing. 
                    list1.GetField().SetFlag(Field.Flag.e_multiselect, true);
                    list1.SetFont(Font.Create(doc, Font.StandardType1Font.e_times_italic));
                    list1.SetTextColor(new ColorPt(1, 0, 0), 3);
                    list1.SetFontSize(28);
                    list1.SetBackgroundColor(new ColorPt(1, 1, 1), 3);
                    list1.RefreshAppearance();
                    blank_page.AnnotPushBack(list1);

                    // RadioButton Widget Creation
                    // Create a radio button group and add three radio buttons in it. 
                    RadioButtonGroup radio_group = RadioButtonGroup.Create(doc, "RadioGroup");
                    RadioButtonWidget radiobutton1 = radio_group.Add(new Rect(140, 410, 190, 460));
                    radiobutton1.SetBackgroundColor(new ColorPt(1, 1, 0), 3);
                    radiobutton1.RefreshAppearance();
                    RadioButtonWidget radiobutton2 = radio_group.Add(new Rect(310, 410, 360, 460));
                    radiobutton2.SetBackgroundColor(new ColorPt(0, 1, 0), 3);
                    radiobutton2.RefreshAppearance();
                    RadioButtonWidget radiobutton3 = radio_group.Add(new Rect(480, 410, 530, 460));
                    // Enable the third radio button. By default the first one is selected
                    radiobutton3.EnableButton();
                    radiobutton3.SetBackgroundColor(new ColorPt(0, 1, 1), 3);
                    radiobutton3.RefreshAppearance();
                    radio_group.AddGroupButtonsToPage(blank_page);

                    // Custom push button annotation creation
                    PushButtonWidget custom_pushbutton1 = PushButtonWidget.Create(doc, new Rect(260, 320, 360, 360));
                    // Set the annotation appearance.
                    custom_pushbutton1.SetAppearance(CreateCustomButtonAppearance(doc, false), Annot.AnnotationState.e_normal);
                    // Create 'SubmitForm' action. The action will be linked to the button.
                    FileSpec url = FileSpec.CreateURL(doc, "http://www.pdftron.com");
                    pdftron.PDF.Action button_action = pdftron.PDF.Action.CreateSubmitForm(url);
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
                    doc.RefreshFieldAppearances();				

					doc.Save(output_path + "forms_test1.pdf", 0);

					Console.WriteLine("Done.");
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

			//----------------------------------------------------------------------------------
			// Example 2: 
			// Fill-in forms / Modify values of existing fields.
			// Traverse all form fields in the document (and print out their names). 
			// Search for specific fields in the document.
			//----------------------------------------------------------------------------------
			try  
			{
				using (PDFDoc doc = new PDFDoc(output_path + "forms_test1.pdf"))
				{
					doc.InitSecurityHandler();

					FieldIterator itr;
					for(itr=doc.GetFieldIterator(); itr.HasNext(); itr.Next()) 
					{
						Field field = itr.Current();
                        string cur_field_name = field.GetName();
                        // Add one to the count for this field name for later processing
                        field_names[cur_field_name] = (field_names.ContainsKey(cur_field_name) ? field_names[cur_field_name] + 1 : 1);

                        Console.WriteLine("Field name: {0}", field.GetName());
						Console.WriteLine("Field partial name: {0}", field.GetPartialName());
                        string str_val = field.GetValueAsString();

						Console.Write("Field type: ");
						Field.Type type = field.GetType();
						switch(type)
						{
						case Field.Type.e_button: 
							Console.WriteLine("Button");
							break;
						case Field.Type.e_radio: 
							Console.WriteLine("Radio button: Value = " + str_val);
							break;
						case Field.Type.e_check: 
							field.SetValue(true);
							Console.WriteLine("Check box: Value = " + str_val);
							break;
						case Field.Type.e_text:
							{
								Console.WriteLine("Text"); 

								// Edit all variable text in the document
								String old_value = "none";
								if (field.GetValue() != null)
									old_value = field.GetValue().GetAsPDFText();

								field.SetValue("This is a new value. The old one was: " + old_value);
							}
							break;
						case Field.Type.e_choice:
							Console.WriteLine("Choice"); 
							break;
						case Field.Type.e_signature:
							Console.WriteLine("Signature"); 
							break;
						}

						Console.WriteLine("------------------------------");
					}

					// Search for a specific field
					Field fld = doc.GetField("employee.name.first");
					if (fld != null) 
					{
						Console.WriteLine("Field search for {0} was successful", fld.GetName());
					}
					else 
					{
						Console.WriteLine("Field search failed.");
					}

					// Regenerate field appearances.
					doc.RefreshFieldAppearances();
					doc.Save(output_path + "forms_test_edit.pdf", 0);
					Console.WriteLine("Done.");
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

			//----------------------------------------------------------------------------------
			// Sample: Form templating
			// Replicate pages and form data within a document. Then rename field names to make 
			// them unique.
			//----------------------------------------------------------------------------------
			try  
			{
				// Sample: Copying the page with forms within the same document
				using (PDFDoc doc = new PDFDoc(output_path + "forms_test1.pdf"))
				{
					doc.InitSecurityHandler();

					Page src_page = doc.GetPage(1);
					doc.PagePushBack(src_page);  // Append several copies of the second page
					doc.PagePushBack(src_page);	 // Note that forms are successfully copied
					doc.PagePushBack(src_page);
                    doc.PagePushBack(src_page);

                    // Now we rename fields in order to make every field unique.
                    // You can use this technique for dynamic template filling where you have a 'master'
                    // form page that should be replicated, but with unique field names on every page. 
                    foreach (KeyValuePair<string, int> cur_field in field_names)
                    {
                        RenameAllFields(doc, cur_field.Key, cur_field.Value);
                    }

                    doc.Save(output_path + "forms_test1_cloned.pdf", 0);
					Console.WriteLine("Done.");
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
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
				using (PDFDoc doc = new PDFDoc(output_path + "forms_test1.pdf"))
				{
					doc.InitSecurityHandler();

					bool auto = true;
					if (auto)
					{
						doc.FlattenAnnotations();
					}
					else  // Manual flattening 
					{
						// Traverse all pages
						PageIterator pitr = doc.GetPageIterator();
						for (; pitr.HasNext(); pitr.Next())
						{
							Page page = pitr.Current();
                            for (int i = page.GetNumAnnots() - 1; i >= 0; --i)
                            {
                                Annot annot = page.GetAnnot(i);
                                if (annot.GetType() == Annot.Type.e_Widget)
                                {
                                    annot.Flatten(page);
                                }
                            }
						}
					}

					doc.Save(output_path + "forms_test1_flattened.pdf", 0);
					Console.WriteLine("Done.");
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}
		}

        // field_nums has to be greater than 0.
        static void RenameAllFields(PDFDoc doc, String name, int field_nums = 1)
		{
			Field fld = doc.GetField(name);
			for (int counter = 1; fld != null; ++counter)
			{
                string field_new_name = name;
                int update_count = System.Convert.ToInt32(Math.Ceiling(counter / (double)field_nums));
                fld.Rename(name + "-" + update_count.ToString());
                fld = doc.GetField(name);
            }
		}

		static Obj CreateCustomButtonAppearance(PDFDoc doc, bool button_down) 
		{
			// Create a button appearance stream ------------------------------------
			using (ElementBuilder builder = new ElementBuilder())
			using (ElementWriter writer = new ElementWriter())
			{
				writer.Begin(doc); 

				// Draw background
				Element element = builder.CreateRect(0, 0, 101, 37);
				element.SetPathFill(true);
				element.SetPathStroke(false);
				element.GetGState().SetFillColorSpace(ColorSpace.CreateDeviceGray());
				element.GetGState().SetFillColor(new ColorPt(0.75, 0.0, 0.0));
				writer.WriteElement(element); 

				// Draw 'Submit' text
				writer.WriteElement(builder.CreateTextBegin()); 
		
				element = builder.CreateTextRun("Submit", Font.Create(doc, Font.StandardType1Font.e_helvetica_bold), 12);
				element.GetGState().SetFillColor(new ColorPt(0, 0, 0));

				if (button_down) 
					element.SetTextMatrix(1, 0, 0, 1, 33, 10);
				else 
					element.SetTextMatrix(1, 0, 0, 1, 30, 13);
				writer.WriteElement(element);
				writer.WriteElement(builder.CreateTextEnd());

				Obj stm = writer.End();

				// Set the bounding box
				stm.PutRect("BBox", 0, 0, 101, 37);
				stm.PutName("Subtype", "Form");
				return stm;
			}
		}
	}
}
