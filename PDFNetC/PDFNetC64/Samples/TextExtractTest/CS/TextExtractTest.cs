//
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
//

using System;
using System.Drawing;
using pdftron;
using pdftron.Common;
using pdftron.Filters;
using pdftron.SDF;
using pdftron.PDF;


namespace TextExtractTestCS
{
	// This sample illustrates various text extraction capabilities of PDFNet.

	class Class1
	{		
		private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static Class1() {}
		
		static void Main(string[] args)
		{
			PDFNet.Initialize();

			// Relative path to the folder containing test files.
			string input_path =  "../../TestFiles/";

			bool example1_basic     = false;
			bool example2_xml       = false;
			bool example3_wordlist  = false;
			bool example4_advanced  = true;
			bool example5_low_level = false;

			// Sample code showing how to use high-level text extraction APIs.
			try	
			{
				using (PDFDoc doc = new PDFDoc(input_path + "newsletter.pdf"))
				{
					doc.InitSecurityHandler();

					Page page = doc.GetPage(1);
					if (page == null) {
						Console.WriteLine("Page not found.");
						return;
					}

					using (TextExtractor txt = new TextExtractor())
					{
						txt.Begin(page);  // Read the page.
						// Other options you may want to consider...
						// txt.Begin(page, null, TextExtractor.ProcessingFlags.e_no_dup_remove);
						// txt.Begin(page, null, TextExtractor.ProcessingFlags.e_remove_hidden_text);
						// ...

						// Example 1. Get all text on the page in a single string.
						// Words will be separated with space or new line characters.
						if (example1_basic) 
						{
							// Get the word count.
							Console.WriteLine("Word Count: {0}", txt.GetWordCount());
						
							Console.WriteLine("\n\n- GetAsText --------------------------\n{0}", txt.GetAsText());
							Console.WriteLine("-----------------------------------------------------------");
						}

						// Example 2. Get XML logical structure for the page.
						if (example2_xml) 
						{
							String text = txt.GetAsXML(TextExtractor.XMLOutputFlags.e_words_as_elements | TextExtractor.XMLOutputFlags.e_output_bbox | TextExtractor.XMLOutputFlags.e_output_style_info);
							Console.WriteLine("\n\n- GetAsXML  --------------------------\n{0}", text);
							Console.WriteLine("-----------------------------------------------------------");
						}

						// Example 3. Extract words one by one.
						if (example3_wordlist) 
						{
							TextExtractor.Word word;
							for (TextExtractor.Line line = txt.GetFirstLine(); line.IsValid(); line=line.GetNextLine())	
							{
								for (word=line.GetFirstWord(); word.IsValid(); word=word.GetNextWord()) 
								{
									Console.WriteLine(word.GetString());
								}
							}
							Console.WriteLine("-----------------------------------------------------------");
						}

						// Example 3. A more advanced text extraction example. 
						// The output is XML structure containing paragraphs, lines, words, 
						// as well as style and positioning information.
						if (example4_advanced) 
						{
							Rect bbox;
							int cur_flow_id=-1, cur_para_id=-1;

							TextExtractor.Line line;
							TextExtractor.Word word;
							TextExtractor.Style s, line_style;

                            Console.WriteLine("<PDFText>");
							// For each line on the page...
							for (line=txt.GetFirstLine(); line.IsValid(); line=line.GetNextLine())
							{
								if (line.GetNumWords() == 0)
								{
									continue;
								}

								if (cur_flow_id != line.GetFlowID()) {
									if (cur_flow_id != -1) {
										if (cur_para_id != -1) {
											cur_para_id = -1;
											Console.WriteLine("</Para>");
										}
										Console.WriteLine("</Flow>");
									}
									cur_flow_id = line.GetFlowID();
									Console.WriteLine("<Flow id=\"{0}\">", cur_flow_id);
								}

								if (cur_para_id != line.GetParagraphID()) {
									if (cur_para_id != -1)
										Console.WriteLine("</Para>");
									cur_para_id = line.GetParagraphID();
									Console.WriteLine("<Para id=\"{0}\">", cur_para_id);
								}	

								bbox = line.GetBBox();
								line_style = line.GetStyle();
								Console.Write("<Line box=\"{0}, {1}, {2}, {3}\"", bbox.x1.ToString("0.00"), bbox.y1.ToString("0.00"), bbox.x2.ToString("0.00"), bbox.y2.ToString("0.00"));
								PrintStyle(line_style);
                                Console.Write(" cur_num=\"" + line.GetCurrentNum() + "\"" + ">\n");

								// For each word in the line...
								for (word=line.GetFirstWord(); word.IsValid(); word=word.GetNextWord())
								{
									// Output the bounding box for the word.
									bbox = word.GetBBox();
									Console.Write("<Word box=\"{0}, {1}, {2}, {3}\"", bbox.x1.ToString("0.00"), bbox.y1.ToString("0.00"), bbox.x2.ToString("0.00"), bbox.y2.ToString("0.00"));
                                    Console.Write(" cur_num=\"" + word.GetCurrentNum() + "\"");
									int sz = word.GetStringLen();
									if (sz == 0) continue;

									// If the word style is different from the parent style, output the new style.
									s = word.GetStyle();
									if (s != line_style) {
										PrintStyle(s);
									}

									Console.Write(">{0}", word.GetString());
									Console.WriteLine("</Word>");
								}
								Console.WriteLine("</Line>");
							}

							if (cur_flow_id != -1) {
								if (cur_para_id != -1) {
									cur_para_id = -1;
									Console.WriteLine("</Para>");
								}
								Console.WriteLine("</Flow>");
							}
						}

					}
					Console.WriteLine("</PDFText>");
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

			// Sample code showing how to use low-level text extraction APIs.
			if (example5_low_level)
			{
				try	
				{
					LowLevelTextExtractUtils util = new LowLevelTextExtractUtils();
					using (PDFDoc doc = new PDFDoc(input_path + "newsletter.pdf"))
					{
						doc.InitSecurityHandler();

						// Example 1. Extract all text content from the document
						using (ElementReader reader = new ElementReader())
						{
							PageIterator itr = doc.GetPageIterator();
							//for (; itr.HasNext(); itr.Next()) //  Read every page
							{				
								reader.Begin(itr.Current());
								LowLevelTextExtractUtils.DumpAllText(reader);
								reader.End();
							}

							// Example 2. Extract text based on the selection rectangle.
							Console.WriteLine("----------------------------------------------------");
							Console.WriteLine("Extract text based on the selection rectangle.");
							Console.WriteLine("----------------------------------------------------");

							Page first_page = doc.GetPage(1);
							string field1 = util.ReadTextFromRect(first_page, new Rect(27, 392, 563, 534), reader);
							string field2 = util.ReadTextFromRect(first_page, new Rect(28, 551, 106, 623), reader);
							string field3 = util.ReadTextFromRect(first_page, new Rect(208, 550, 387, 621), reader);

							Console.WriteLine("Field 1: {0}", field1);
							Console.WriteLine("Field 2: {0}", field2);
							Console.WriteLine("Field 3: {0}", field3);
							// ... 

							Console.WriteLine("Done.");
						}
					}
				}
				catch (PDFNetException e)
				{
					Console.WriteLine(e.Message);
				}	
			}

		}

		static void PrintStyle(TextExtractor.Style s) {
            int[] rgb = s.GetColor();
            String rgb_hex = String.Format("{0:X02}{1:X02}{2:X02};", rgb[0], rgb[1], rgb[2]);
            Console.Write(" style=\"font-family:{0}; font-size:{1};{2} color:#{3}\"", s.GetFontName(), s.GetFontSize(), (s.IsSerif() ? " sans-serif;" : ""), rgb_hex); 
		}
	}

	class LowLevelTextExtractUtils
	{
		// A utility method used to dump all text content in the 
		// console window.
		public static void DumpAllText(ElementReader reader) 
		{
			Element element; 
			while ((element = reader.Next()) != null)
			{
				switch (element.GetType()) 
				{
					case Element.Type.e_text_begin:
						Console.WriteLine("\n--> Text Block Begin");
						break;
					case Element.Type.e_text_end:
						Console.WriteLine("\n--> Text Block End");
						break;
					case Element.Type.e_text:
					{
						Rect bbox = new Rect();
						element.GetBBox(bbox);
						// Console.WriteLine("\n--> BBox: {0}, {1}, {2}, {3}", bbox.x1, bbox.y1, bbox.x2, bbox.y2);

						String txt = element.GetTextString();
						Console.Write(txt);
						Console.WriteLine("");
						break;
					}
					case Element.Type.e_text_new_line:
					{
						// Console.WriteLine("\n--> New Line");
						break;
					}
					case Element.Type.e_form: // Process form XObjects
					{
						reader.FormBegin(); 
						DumpAllText(reader);
						reader.End(); 
						break; 
					}
				}
			}
		}


		private string _srch_str;

		// A helper method for ReadTextFromRect
		void RectTextSearch(ElementReader reader, Rect pos) 
		{			
			Element element; 
			while ((element = reader.Next()) != null)
			{
				switch (element.GetType()) 
				{
					case Element.Type.e_text:
					{
						Rect bbox = new Rect();
						element.GetBBox(bbox);
						if(bbox.IntersectRect(bbox, pos))
						{
							_srch_str += element.GetTextString();
							_srch_str += "\n"; // add a new line?
						}
						break;
					}
					case Element.Type.e_text_new_line:
					{
						break;
					}
					case Element.Type.e_form: // Process form XObjects
					{
						reader.FormBegin(); 
						RectTextSearch(reader, pos);
						reader.End(); 
						break; 
					}
				}
			}
		}

		// A utility method used to extract all text content from
		// a given selection rectangle. The rectangle coordinates are
		// expressed in PDF user/page coordinate system.
		public string ReadTextFromRect(Page page, Rect pos, ElementReader reader)
		{
			_srch_str = "";
			reader.Begin(page);
			RectTextSearch(reader, pos);
			reader.End();
			return _srch_str;
		}
	}
}
