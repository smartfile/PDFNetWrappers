//
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
//

using System;
using pdftron;
using pdftron.Common;
using pdftron.Filters;
using pdftron.SDF;
using pdftron.PDF;


namespace TextSearchTestCS
{
	// This sample illustrates various text search capabilities of PDFNet.

	class Class1
	{		
		private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static Class1() {}
		
		static void Main(string[] args)
		{
			PDFNet.Initialize();

			// Relative path to the folder containing test files.
			string input_path =  "../../TestFiles/";

			// Sample code showing how to use high-level text extraction APIs.
			try	
			{
				using (PDFDoc doc = new PDFDoc(input_path + "credit card numbers.pdf"))
				{
					doc.InitSecurityHandler();

					Int32 page_num = 0;
					String result_str = "", ambient_string = "";
					Highlights hlts = new Highlights();

					TextSearch txt_search = new TextSearch();
					Int32 mode = (Int32)(TextSearch.SearchMode.e_whole_word | TextSearch.SearchMode.e_page_stop | TextSearch.SearchMode.e_highlight);
					String pattern = "joHn sMiTh";

					//call Begin() method to initialize the text search.
					txt_search.Begin( doc, pattern, mode, -1, -1 );

					int step = 0;
			
					//call Run() method iteratively to find all matching instances.
					while ( true )
					{
						TextSearch.ResultCode code = txt_search.Run(ref page_num, ref result_str, ref ambient_string, hlts );

						if ( code == TextSearch.ResultCode.e_found )
						{
							if ( step == 0 )
							{	//step 0: found "John Smith"
								//note that, here, 'ambient_string' and 'hlts' are not written to, 
								//as 'e_ambient_string' and 'e_highlight' are not set.
								Console.WriteLine(result_str + "'s credit card number is: ");

								//now switch to using regular expressions to find John's credit card number
								mode = txt_search.GetMode();
								mode |= (Int32)(TextSearch.SearchMode.e_reg_expression | TextSearch.SearchMode.e_highlight);
								txt_search.SetMode(mode);
								pattern = "\\d{4}-\\d{4}-\\d{4}-\\d{4}"; //or "(\\d{4}-){3}\\d{4}"
								txt_search.SetPattern(pattern);

								++step;
							}
							else if ( step == 1 )
							{
								//step 1: found John's credit card number
								//result_str.ConvertToAscii(char_buf, 32, true);
								//cout << "  " << char_buf << endl;
								Console.WriteLine("  " + result_str);

								//note that, here, 'hlts' is written to, as 'e_highlight' has been set.
								//output the highlight info of the credit card number
								hlts.Begin(doc);
								while (hlts.HasNext())
								{
									Console.WriteLine("The current highlight is from page: " + hlts.GetCurrentPageNumber());
									hlts.Next();
								}

								//see if there is an AMEX card number
								pattern = "\\d{4}-\\d{6}-\\d{5}";
								txt_search.SetPattern(pattern);

								++step;
							}
							else if ( step == 2 )
							{
								//found an AMEX card number
								Console.WriteLine("\nThere is an AMEX card number:\n  " + result_str);

								//change mode to find the owner of the credit card; supposedly, the owner's
								//name proceeds the number
								mode = txt_search.GetMode();
								mode |= (Int32)(TextSearch.SearchMode.e_search_up);
								txt_search.SetMode(mode);
								pattern = "[A-z]++ [A-z]++";
								txt_search.SetPattern(pattern);

								++step;
							}
							else if ( step == 3 )
							{
								//found the owner's name of the AMEX card
								Console.WriteLine("Is the owner's name:\n  " + result_str + "?");

								//add a link annotation based on the location of the found instance
								hlts.Begin(doc);
								while (hlts.HasNext())
								{
									Page cur_page = doc.GetPage(hlts.GetCurrentPageNumber());
									double[] quads = hlts.GetCurrentQuads();
									int quad_count = quads.Length / 8;
									for (int i = 0; i < quad_count; ++i)
									{
										//assume each quad is an axis-aligned rectangle
										int offset = 8 * i;
										double x1 = Math.Min(Math.Min(Math.Min(quads[offset + 0], quads[offset + 2]), quads[offset + 4]), quads[offset + 6]);
										double x2 = Math.Max(Math.Max(Math.Max(quads[offset + 0], quads[offset + 2]), quads[offset + 4]), quads[offset + 6]);
										double y1 = Math.Min(Math.Min(Math.Min(quads[offset + 1], quads[offset + 3]), quads[offset + 5]), quads[offset + 7]);
										double y2 = Math.Max(Math.Max(Math.Max(quads[offset + 1], quads[offset + 3]), quads[offset + 5]), quads[offset + 7]);

										pdftron.PDF.Annots.Link hyper_link = pdftron.PDF.Annots.Link.Create(doc, new Rect(x1, y1, x2, y2), pdftron.PDF.Action.CreateURI(doc, "http://www.pdftron.com"));
										hyper_link.RefreshAppearance();
										cur_page.AnnotPushBack(hyper_link);
									}
									hlts.Next();
								}
								string output_path = "../../TestFiles/Output/";
								doc.Save(output_path + "credit card numbers_linked.pdf", SDFDoc.SaveOptions.e_linearized);

								break;
							}
						}
						else if ( code == TextSearch.ResultCode.e_page )
						{
							//you can update your UI here, if needed
						}
						else
						{
							break;
						}
					}
				}
			}

			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}
		}
	}
}
