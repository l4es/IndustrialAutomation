/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

#include "general_defines.h"
#include "gigaclient.h"

int Giga_Thread::allocate_bindings()
{
	IT_IT("Giga_Thread::allocate_bindings");
	
	if(server_is_connected && db_online)
	{
		if(tables)
		{
			free(tables);
			tables = NULL;
		}

		QSNumberOfTables = cli_show_tables(session_handle, &tables);

		if(QSNumberOfTables)

		if(QSNumberOfTables < 0)
		{
			IT_COMMENT1("cli_show_tables failed with code %d", QSNumberOfTables);
			error_message += QString("cli_show_tables failed with code ") + QString(error_list[QSNumberOfTables*(-1)].name) + " ";
			fFail = true;
			unrecovableError = true;
			return -1;
		}
				
		for(int i = 0; i < QSNumberOfTables; i++)
		{
			if(fields)
			{	
				free(fields);
				fields = NULL;
			}
			
			nCols = cli_describe(session_handle, (char const*)(tables[i].name), &fields);
			if(nCols < 0)
			{
				IT_COMMENT1("cli_describe01 failed with code %d", nCols);	
				//la riga successiva era commentata
				error_message += QString("cli_describe failed with code ") + QString(error_list[nCols*(-1)].name) + " ";
				fFail = true;
				unrecovableError = true;
				return -1;
			}
			else
			{
				if(nCols > max_numCol)
				{
					max_numCol = nCols;
				}
			}
		}

		if(nCols > NMAX_FIELDS_IN_TABLE)
		{
			return -1;
		}

		value_asciiz = new char*[max_numCol];
		if(value_asciiz == NULL) return -1;
		value_real8 = new cli_real8_t[max_numCol];
		if(value_real8 == NULL) return -1;
		value_real4 = new cli_real4_t[max_numCol];
		if(value_real4 == NULL) return -1;
		value_int4 = new cli_int4_t[max_numCol];
		if(value_int4 == NULL) return -1;
		value_int8 = new cli_int8_t[max_numCol];
		if(value_int8 == NULL) return -1;
		value_oid = new cli_oid_t[max_numCol];
		if(value_oid == NULL) return -1;

		updated_field_name = new char*[max_numCol];
		if(updated_field_name == NULL) return -1;
		new_field_value = new char*[max_numCol];
		if(new_field_value == NULL) return -1;
		selected_field_name = new char*[max_numCol];
		if(selected_field_name == NULL) return -1;
		selected_field_name2 = new char*[max_numCol];
		if(selected_field_name2 == NULL) return -1;
		
		for(i = 0; i < max_numCol; i++)
		{
			value_asciiz[i] = new char[MAX_LENGHT_OF_STRING];
			if(value_asciiz[i] == NULL) return -1;
			updated_field_name[i] = new char[MAX_LENGHT_OF_STRING];
			if(updated_field_name[i] == NULL) return -1;
			new_field_value[i] = new char[MAX_LENGHT_OF_STRING];
			if(new_field_value[i] == NULL) return -1;
			selected_field_name[i] = new char[MAX_LENGHT_OF_STRING];
			if(selected_field_name[i] == NULL) return -1;
			selected_field_name2[i] = new char[MAX_LENGHT_OF_STRING];
			if(selected_field_name2[i] == NULL) return -1;
		}
	}

	return 0;
}

void Giga_Thread::deallocate_bindings()
{
	IT_IT("Giga_Thread::deallocate_bindings");
	
	if(server_is_connected && db_online)
	{
		if(value_asciiz == NULL)
			return;
		
		if(value_real8 == NULL)
			return;
		
		if(value_real4 == NULL)
			return;
		
		if(value_int4 == NULL)
			return;
		
		if(value_oid == NULL)
			return;
		
		if(value_int8 == NULL)
			return;
		
		if(updated_field_name == NULL)
			return;
		
		if(new_field_value == NULL)
			return;
		
		if(selected_field_name == NULL)
			return;
		
		if(selected_field_name2 == NULL)
			return;
					
		
		for(int i = 0; i < max_numCol; i++)
		{
			if(value_asciiz[i])
			delete[] value_asciiz[i];
			if(updated_field_name[i])
			delete[] updated_field_name[i];
			if(new_field_value[i])
			delete[] new_field_value[i];
			if(selected_field_name[i])
			delete[] selected_field_name[i];
			if(selected_field_name2[i])
			delete[] selected_field_name2[i];
		}

		if(value_asciiz)
		delete[] value_asciiz;
		if(value_real8)
		delete[] value_real8;
		if(value_real4)
		delete[] value_real4;
		if(value_int4)
		delete[] value_int4;
		if(value_oid)
		delete[] value_oid;
		if(value_int8)
		delete[] value_int8;
		if(updated_field_name)
		delete[] updated_field_name;
		if(new_field_value)
		delete[] new_field_value;
		if(selected_field_name)
		delete[] selected_field_name;
		if(selected_field_name2)
		delete[] selected_field_name2;

		if(tables)
		{
			free(tables);
			tables = NULL;
		}
		if(fields)
		{	
			free(fields);
			fields = NULL;
		}
	}
}


bool Giga_Thread::SetQuery(const QString &s)
{	
	IT_IT("Giga_Thread::SetQuery");
	
	if(Done) 
	{
		strcpy(giga_query, (const char*)(s.stripWhiteSpace())); // set the query 
		Trigger = true; // go 
		Done = false; // mark as not complete
				
		m_hevtSinc.signal();
		return true;
	}
	return false;
}	

/*
*Function:run
*runs the thread
*Inputs:none
*Outputs:none
*Returns:none
*/

void Giga_Thread::run()
{	
	IT_IT("Giga_Thread::run");

	int rc = allocate_bindings();

	if(rc < 0)
	{
		fFail = true;
		IT_COMMENT1("allocate_bindings01 failed with code %d", rc);
		deallocate_bindings();
		return;
	}
	
	IT_COMMENT1("Giga_Thread is Running, connected to %s", (const char*)(Parent->GetName()));
	
	while(true)
	{
		IT_COMMENT("Giga_Thread is Waiting....");
		
		{
			dbCriticalSection cs(mutex); //enter critical section
			m_hevtSinc.wait(mutex);
			if(!fExit)
			{
				m_hevtSinc.reset();
			}
		}//exit critical section
		
		if(fExit)
		{
			IT_COMMENT("Giga_Thread is Exiting ....");
			deallocate_bindings();
			m_hevtEnd.signal();
			return; //terminate the thread
		}
		
		IT_COMMENT("Giga_Thread Past Wait Flag");
		if(Trigger)
		{
			if(pLastResult)
			{
				delete pLastResult; // clear last result
				pLastResult = NULL;
			};
			//
			Trigger = false;

			db_message = QObject::tr("Query: [") + QString(giga_query) + "]  ";
			
			{
				///START TRANSACTION/////////////////////////////////////////////////////////
								
				IT_COMMENT1("Query: %s", giga_query);
								
				QString condition, query_buf;
				QString table_name;
															
				int j, k;
				
				int main_token;
				bool star = false;
				pos = 0;  //points to che current char into the query string
				main_token = scan();

				switch(main_token) 
				{
					case tkn_select:
					{
						if(!db_online)break;

						int nFieldsSelected;
						int tk;
						tk = 0;

						pLastResult = new Giga_Result;
						pLastResult->Tuples.resize(limit_of_records);

						bool loop = true;
						while(loop)
						{
							switch(scan())
							{
								case tkn_all:
								star = true;
								//IT_COMMENT("seltoken = '*'");
								break;
								case tkn_from:
									//IT_COMMENT1("seltoken = %s", buf);
								loop = false;
								break;
								case tkn_ident:
								{
									//I get a field name
									//IT_COMMENT1("seltoken = %s", buf);
									strcpy(selected_field_name[tk], buf);
									tk += 1;
									pLastResult->fieldNames << QString(buf); 
								}
								break;
								case tkn_comma:
									//IT_COMMENT("seltoken = ','");
								break;
								default:
									error("Error in select scan");
									//IT_COMMENT1("seltoken = %s", buf);
								break;
								
							}
						}
						
						nFieldsSelected = tk;
						
						if (scan() != tkn_ident) 
						{
							error("Table name expected");
						}
						
						table_name = name;

						int tkn;
						if((tkn = scan()) == tkn_where)
						{
							query_buf = nested_select(table_name);
						}
						else if(tkn == tkn_null)
						{
							query_buf = "select * from "+ table_name;
						}
						else if(tkn == tkn_order)
						{
							readCondition(); 
							condition = buf;
							query_buf = "select * from "+ table_name +" order" + condition;
						}
						else if(tkn == tkn_semi)
						{
							condition = buf;
							query_buf = "select * from "+ table_name;
						}
						else if(tkn == tkn_limit)
						{
							readCondition(); 
							condition = buf;
							query_buf = "select * from "+ table_name +" limit" + condition;
						}
						else 
						{
							IT_COMMENT("query failed: Unsupported ReadCondition");	
							error_message += QString("query failed: Unsupported ReadCondition") + " ";
							fFail = true;
							break;
						}

						IT_COMMENT1("Modified query: %s", (const char *)query_buf);
						statement = cli_statement(session_handle, (const char *)query_buf);		
						
						if (statement >= 0)
						{
							//select

							if(fields)
							{	
								free(fields);
								fields = NULL;
							}
							
							nCols = cli_describe(session_handle, (char const*) (table_name), &fields);
							if(nCols < 0)
							{
								IT_COMMENT1("cli_describe02 failed with code %d", nCols);	
								//la riga successiva era commentata
								error_message += QString("cli_describe failed with code ") + QString(error_list[nCols*(-1)].name) + " ";
								fFail = true;
								unrecovableError = true;
								break;
							}
							else
							{
								if(nCols > max_numCol)
								{
									deallocate_bindings();
									if(allocate_bindings())
									{assert(0); break;}
								}
							}
							
							//binding delle colonne
							for(j = 0; j < nCols; j++)
							{
								if(star)
								{
									pLastResult->fieldNames << QString(fields[j].name); 
								}
								
								switch(fields[j].type)
								{
									case cli_asciiz:/* zero terminated string */
									{
										rc = cli_column(statement, fields[j].name, cli_asciiz, NULL, value_asciiz[j]);
										if(rc != cli_ok)
										{
											IT_COMMENT1("cli_column failed with code %d in cli_asciiz case", rc);
											error_message += QString("cli_column failed with code ") + QString(error_list[rc*(-1)].name) + " ";
											fFail = true;
											break;
										}
									}
									break;
									case cli_real8:
									{
										rc = cli_column(statement, fields[j].name, cli_real8, NULL, &value_real8[j]);
										if(rc != cli_ok)
										{
											IT_COMMENT1("cli_column failed with code %d in cli_real8 case", rc);
											error_message += QString("cli_column failed with code ") + QString(error_list[rc*(-1)].name) + " ";
											fFail = true;
											break;
										}
									}
									break;
									case cli_real4:
									{
										rc = cli_column(statement, fields[j].name, cli_real4, NULL, &value_real4[j]);
										if(rc != cli_ok)
										{
											IT_COMMENT1("cli_column failed with code %d in cli_real4 case", rc);
											error_message += QString("cli_column failed with code ") + QString(error_list[rc*(-1)].name) + " ";
											fFail = true;
											break;
										}
									}
									break;
									case cli_int4:
									{
										rc = cli_column(statement, fields[j].name, cli_int4, NULL, &value_int4[j]);
										if(rc != cli_ok)
										{
											IT_COMMENT1("cli_column failed with code %d in cli_int4 case", rc);
											error_message += QString("cli_column failed with code ") + QString(error_list[rc*(-1)].name) + " ";
											fFail = true;
											break;
										}
									}
									break;
									case cli_oid:
									{
										rc = cli_column(statement, fields[j].name, cli_oid, NULL, &value_oid[j]);
										if(rc != cli_ok)
										{
											IT_COMMENT1("cli_column failed with code %d in cli_oid case", rc);
											error_message += QString("cli_column failed with code ") + QString(error_list[rc*(-1)].name) + " ";
											fFail = true;
											break;
										}
									}
									break;
									case cli_int8:
									{
										rc = cli_column(statement, fields[j].name, cli_int8, NULL, &value_int8[j]);
										if(rc != cli_ok)
										{
											IT_COMMENT1("cli_column failed with code %d in cli_int8 case", rc);
											error_message += QString("cli_column failed with code ") + QString(error_list[rc*(-1)].name) + " ";
											fFail = true;
											break;
										}
									}
									break;
									default:
									unupported_types(fields[j].type);
									break;
								}
							}

							rc = cli_fetch(statement, cli_view_only);
							if (rc < 0) 
							{
								IT_COMMENT1("cli_fetch failed with code %d",rc);
								error_message += QString("cli_fetch failed with code ") + QString(error_list[rc*(-1)].name) + " ";
								fFail = true;
								break;
							}
							
							//FETCH
							
							QStringList *pT;
							long size = pLastResult->Tuples.size();

							for (k = 0;((rc = cli_get_next(statement)) == cli_ok) ; k++) 
							{
								//IT_COMMENT1("record # %d", k);
							
								pT = new QStringList;
							
								if(k >= size)
								{
									pLastResult->Tuples.resize(pLastResult->Tuples.size() * 2); // double the size of the vector
									size = pLastResult->Tuples.size();
								}
								
								pLastResult->Tuples.insert(k,pT);
																													
								for(j = 0; j < nCols; j++)
								{
									if(nFieldsSelected)
									{
										for(int tk = 0; tk < nFieldsSelected; tk++)
										{
											if(!strcmp(fields[j].name, selected_field_name[tk]))
											{
												switch(fields[j].type)
												{
													case cli_asciiz:
													{
														*pT << QString(value_asciiz[j]);
														//IT_COMMENT2("field %s cli_asciiz: %s", fields[j].name, value_asciiz[j]);
														tk = nFieldsSelected; //serve a finire il ciclo!
														
													}
													break;
													case cli_real8:
													{
														*pT << QString::number(((cli_real8_t)	value_real8[j]));
														//IT_COMMENT2("field %s cli_real8: %lf", fields[j].name, value_real8[j]);
														tk = nFieldsSelected; //serve a finire il ciclo!
														
													}
													break;
													case cli_real4:
													{
														*pT << QString::number(((cli_real4_t)	value_real4[j]));
														//IT_COMMENT2("field %s cli_real4: %lf", fields[j].name, value_real4[j]);
														tk = nFieldsSelected; //serve a finire il ciclo!
														
													}
													break;
													case cli_int4:
													{
														*pT << QString::number(((cli_int4_t)	value_int4[j]));
														//IT_COMMENT2("field %s cli_int4:  %d", fields[j].name, value_int4[j]);
														tk = nFieldsSelected; //serve a finire il ciclo!
														
													}
													break;
													case cli_oid:
													{
														*pT << QString::number(((cli_oid_t)	value_oid[j]));
														//IT_COMMENT2("field %s cli_oid:  %d", fields[j].name, value_oid[j]);
														tk = nFieldsSelected; //serve a finire il ciclo!
													
													}
													break;
													case cli_int8:
													{
														char buffer[20];
														_i64toa(value_int8[j], buffer, 10);
														*pT << QString(buffer);
														//IT_COMMENT2("field %s cli_int8:  %d", fields[j].name, value_int8[j]);
														tk = nFieldsSelected; //serve a finire il ciclo!
														
													}
													break;
													default:
														*pT << QString(NONE_STR);
														unupported_types(fields[j].type);
													break;
												}
											}
										}
									}
									else
									{
										//select *

										switch(fields[j].type)
										{
											case cli_asciiz:
											{
												*pT << QString(value_asciiz[j]);
												//IT_COMMENT2("field %s cli_asciiz: %s", fields[j].name, value_asciiz[j]);
												
											}
											break;
											case cli_real8:
											{
												*pT << QString::number(((cli_real8_t)	value_real8[j]));
												//IT_COMMENT2("field %s cli_real8: %lf", fields[j].name, value_real8[j]);
												
											}
											break;
											case cli_real4:
											{
												*pT << QString::number(((cli_real4_t)	value_real4[j]));
												//IT_COMMENT2("field %s cli_real4: %lf", fields[j].name, value_real4[j]);
												
											}
											break;
											case cli_int4:
											{
												*pT << QString::number(((cli_int4_t)	value_int4[j]));
												//IT_COMMENT2("field %s cli_int4:  %d", fields[j].name, value_int4[j]);
												
											}
											break;
											case cli_oid:
											{
												*pT << QString::number(((cli_oid_t)	value_oid[j]));
												//IT_COMMENT2("field %s cli_oid:  %d", fields[j].name, value_oid[j]);
												
											}
											break;
											case cli_int8:
											{
												char buffer[20];
												_i64toa(value_int8[j], buffer, 10);
												*pT << QString(buffer);
												//IT_COMMENT2("field %s cli_int8:  %d", fields[j].name, value_int8[j]);
												
											}
											break;
											default:
											*pT << QString(NONE_STR);
											unupported_types(fields[j].type);
											break;
										}
										//end select *
									} //end else
								}
								
							}//end for
							
							if(!k && (rc == cli_not_found))
							{
								IT_COMMENT("Nessun record in database");
								db_message += QString(QObject::tr(" No records fetched "));
								if(pLastResult)
								{
									delete pLastResult; // clear last result
									pLastResult = NULL;
								};
							}
							else
							{
								IT_COMMENT1("Number of records fetched: %d", k);
								db_message += QString(QObject::tr(" Number of records fetched: ")) + QString::number(k) + " ";
							}
																			
							//free the statement
							rc = cli_free(statement);
							if(rc != cli_ok)
							{
								IT_COMMENT1("cli_free failed with code %d", rc);
								error_message += QString("cli_free failed with code ") + QString(error_list[rc*(-1)].name) + " ";
								fFail = true;
								unrecovableError = true;
								break;
							}

							rc = cli_commit(session_handle);
							if(rc != cli_ok)  
							{
								IT_COMMENT1("cli_commit01 failed with code %d", rc);
								error_message += QString("cli_commit failed with code ") + QString(error_list[rc*(-1)].name) + " ";
								fFail = true;
								unrecovableError = true;
								break;
							}
						}
						else
						{
							IT_COMMENT1("cli_statement failed with code %d", statement);
							error_message += QString("cli_statement failed with code ") + QString(error_list[statement*(-1)].name) + " ";
							fFail = true;
							break;
						}
					}
				    break;	
				  
				    case tkn_insert:
					{
						if(!db_online)break;

						if (expect("into", tkn_into) && expect("table name", tkn_ident)) 
						{
							table_name = name;

							if(fields)
							{	
								free(fields);
								fields = NULL;
							}
							
							nCols = cli_describe(session_handle, (char const*) table_name, &fields);

							if(nCols < 0)
							{
								IT_COMMENT1("cli_describe03 failed with code %d", nCols);	
								error_message += QString("cli_describe failed with code ") + QString(error_list[nCols*(-1)].name) + " ";
								fFail = true;
								unrecovableError = true;
								break;
							}
							else
							{
								if(nCols > max_numCol)
								{
									deallocate_bindings();
									if(allocate_bindings())
									{assert(0);break;}
								}
							}

							for(int tk = 0; tk < nCols; tk++)
							{
								if(new_field_value)//30-11-09
									new_field_value[tk][0] = '\0';
								if(updated_field_name)//30-11-09
									updated_field_name[tk][0] = '\0';
							}
							

							int token, nNewFields;
							token = scan();

							if(token == tkn_lpar)
							{
								int tk;

								tk = 0;

								bool loop = true;
								while (loop) 
								{
									switch (scan()) 
									{
									  case tkn_rpar:
											break;
									  case tkn_values:
											loop = false;
											break;
									  case tkn_comma:
											break;
									  case tkn_ident:
										  {
											strcpy(updated_field_name[tk], buf);
											tk +=1;
											break;
										  }
									  default:
									  error("Syntax error in insert, first loop");
									  loop = false;
									  break;
									}
								}
							}
							else
							{
								for(int tk = 0; tk < nCols; tk++)
								{
									strcpy(updated_field_name[tk], fields[tk].name);
								}
							}
															
							if (expect("(", tkn_lpar)) 
							{
								////insert record/////////////////////////////////

								QString NewQuery;
								NewQuery = "insert into "+ table_name;  

								statement = cli_statement(session_handle, (const char *)NewQuery);		
								if (statement >= 0)
								{
									/////////read values////////////////
									int tkn;
									
									j = 0;
									int tk = 0;
									while (true) 
									{
										switch (scan()) 
										{
										  case tkn_rpar:
								    		break;
										  case tkn_iconst:
											value_int4[j] = ival;
											value_int8[j] = ival;
											strcpy(new_field_value[tk], numbuf);
											tk += 1;
											j += 1;
											break;
										  case tkn_fconst:
											value_real8[j] = fval;
											strcpy(new_field_value[tk], numbuf);
											tk += 1;
											j += 1;
											break;
										  case tkn_sconst:
											strcpy(value_asciiz[j], buf);
											strcpy(new_field_value[tk], buf);
											tk += 1;
											j += 1;
											break;
										  case tkn_error:
											break;
										  default:
											error("Syntax error in insert");
											break;
										}
										
										if ((tkn = scan()) == tkn_rpar) 
										{
											break;
										}

										if (tkn != tkn_comma) 
										{
											error("',' expected");
											break;
										}
									}

									nNewFields = tk;
									///////////read values//////////////////

									//binding delle colonne
									for(j = 0; j < nCols; j++)
									{
										switch(fields[j].type)
										{
											case cli_asciiz:
											{
												rc = cli_column(statement, fields[j].name, cli_asciiz, NULL, value_asciiz[j]);
												if(rc != cli_ok)
												{
													IT_COMMENT1("cli_column failed with code %d in cli_asciiz case", rc);
													error_message += QString("cli_column failed with code ") + QString(error_list[rc*(-1)].name) + " ";
													fFail = true;
													break;
												}
												value_asciiz[j][0] = '\0';
												
											}
											break;
											case cli_real8:
											{
												rc = cli_column(statement, fields[j].name, cli_real8, NULL, &value_real8[j]);
												if(rc != cli_ok)
												{
													IT_COMMENT1("cli_column failed with code %d in cli_real8 case", rc);
													error_message += QString("cli_column failed with code ") + QString(error_list[rc*(-1)].name) + " ";
													fFail = true;
													break;
												}
												value_real8[j] = 0.0;
												
											}
											break;
											case cli_real4:
											{
												rc = cli_column(statement, fields[j].name, cli_real4, NULL, &value_real4[j]);
												if(rc != cli_ok)
												{
													IT_COMMENT1("cli_column failed with code %d in cli_real4 case", rc);
													error_message += QString("cli_column failed with code ") + QString(error_list[rc*(-1)].name) + " ";
													fFail = true;
													break;
												}
												value_real4[j] = 0.0;
											}
											break;
											case cli_int4:
											{
												rc = cli_column(statement, fields[j].name, cli_int4, NULL, &value_int4[j]);
												if(rc != cli_ok)
												{
													IT_COMMENT1("cli_column failed with code %d in cli_int4 case", rc);
													error_message += QString("cli_column failed with code ") + QString(error_list[rc*(-1)].name) + " ";
													fFail = true;
													break;
												}
												value_int4[j] = 0;
												
											}
											break;
											case cli_oid:
											{
												rc = cli_column(statement, fields[j].name, cli_oid, NULL, &value_oid[j]);
												if(rc != cli_ok)
												{
													IT_COMMENT1("cli_column failed with code %d in cli_oid case", rc);
													error_message += QString("cli_column failed with code ") + QString(error_list[rc*(-1)].name) + " ";
													fFail = true;
													break;
												}
												value_oid[j] = 0;
											}
											break;
											case cli_int8:
											{
												rc = cli_column(statement, fields[j].name, cli_int8, NULL, &value_int8[j]);
												if(rc != cli_ok)
												{
													IT_COMMENT1("cli_column failed with code %d in cli_int8 case", rc);
													error_message += QString("cli_column failed with code ") + QString(error_list[rc*(-1)].name) + " ";
													fFail = true;
													break;
												}
												value_int8[j] = 0;
											}
											break;
											default:
											unupported_types(fields[j].type);
											break;
										}
									}
							
									//assegnazione delle variabili
									
									for(j = 0; j < nCols; j++)
									{
										for(tk = 0; tk < nNewFields; tk++)
										{	
											if(!strcmp(fields[j].name, updated_field_name[tk]))
											{
												switch(fields[j].type)
												{
													case cli_asciiz:
													{
														strcpy(value_asciiz[j], (const char*)new_field_value[tk]);
														//IT_COMMENT2("field %s cli_asciiz: %s", fields[j].name, value_asciiz[j]);
														tk = nNewFields; //serve a finire il ciclo!															
													}
													break;
													case cli_real8:
													{
														value_real8[j] = (cli_real8_t)atof((const char*)new_field_value[tk]);
														//IT_COMMENT2("field %s cli_real8: %lf", fields[j].name, value_real8[j]);
														tk = nNewFields; //serve a finire il ciclo!
														
													}
													break;
													case cli_real4:
													{
														value_real4[j] = (cli_real4_t)atof((const char*)new_field_value[tk]);
														//IT_COMMENT2("field %s cli_real4: %lf", fields[j].name, value_real4[j]);
														tk = nNewFields; //serve a finire il ciclo!
														
													}
													break;
													case cli_int4:
													{
														value_int4[j] = (cli_int4_t)atoi((const char*)new_field_value[tk]);
														//IT_COMMENT2("field %s cli_int4:  %d", fields[j].name, value_int4[j]);
														tk = nNewFields; //serve a finire il ciclo!
														
													}
													break;
													case cli_oid:
													{
														value_oid[j] = (cli_oid_t)atol((const char*)new_field_value[tk]);
														//IT_COMMENT2("field %s cli_oid:  %d", fields[j].name, value_oid[j]);
														tk = nNewFields; //serve a finire il ciclo!
													}
													break;
													case cli_int8:
													{
														value_int8[j] = (cli_int8_t)_atoi64((const char*)new_field_value[tk]);
														//IT_COMMENT2("field %s cli_int8:  %d", fields[j].name, value_int8[j]);
														tk = nNewFields; //serve a finire il ciclo!
													}
													break;
													default:
													unupported_types(fields[j].type);
													break;
												}
											}
										}
									}
									
									rc = cli_insert(statement, &oid);
																											
									//free the statement
									rc = cli_free(statement);
									if(rc != cli_ok)
									{
										IT_COMMENT1("cli_free failed with code %d", rc);
										error_message += QString("cli_free failed with code ") + QString(error_list[rc*(-1)].name) + " ";
										fFail = true;
										unrecovableError = true;
										break;
									}
																		

									if(low_freq_flag || (num_recors_inserted == (unsigned)commit_freq))
									{
										num_recors_inserted = 0;
										
										rc = cli_commit(session_handle);
										if(rc != cli_ok)
										{
											IT_COMMENT1("cli_commit02 failed with code %d", rc);
											error_message += QString("cli_commit failed with code ") + QString(error_list[rc*(-1)].name) + " ";
											fFail = true;
											unrecovableError = true;
											break;
										}
									}
									else
									{
										num_recors_inserted++;

										rc = cli_precommit(session_handle);
										if(rc != cli_ok)  
										{
											IT_COMMENT1("cli_precommit failed with code %d", rc);
											error_message += QString("cli_precommit failed with code ") + QString(error_list[rc*(-1)].name) + " ";
											fFail = true;
											break;
										}
									}
								}
								else
								{
									IT_COMMENT1("cli_statement failed with code %d", statement);
									error_message += QString("cli_statement failed with code ") + QString(error_list[statement*(-1)].name) + " ";
									fFail = true;
									break;
								}

							}
						}
					}
					break;

					case tkn_delete:
					{
						if(!db_online)break;

						if (expect("from", tkn_from) && expect("table name", tkn_ident)) 
						{
					   		int tkn;
																					
							table_name = name;

							if((tkn = scan()) == tkn_where)
							{
								query_buf = nested_select(table_name);
							}
							else
							{
								query_buf = "select * from "+ table_name;
							}
							
							IT_COMMENT1("Modified query: %s", (const char *)query_buf);							
							statement = cli_statement(session_handle, (const char *)query_buf);		
												
							if (statement >= 0)
							{
								//delete
								
								rc = cli_fetch(statement, cli_for_update);
								
								if(rc == cli_not_found)
								{
									IT_COMMENT("Nessun record in database");
									db_message += QString(QObject::tr(" No records fetched "));
								}
								else if(rc > 0)
								{
									rc = cli_remove(statement);

									if(rc != cli_ok && rc != cli_not_found)
									{
										IT_COMMENT1("cli_remove failed with code %d", rc);
										error_message += QString("cli_remove failed with code ") + QString(error_list[rc*(-1)].name) + " ";
										fFail = true;
										break;
									}
								}
								else if (rc < 0) 
								{
									IT_COMMENT1("cli_fetch failed with code %d",rc);
									error_message += QString("cli_fetch failed with code ") + QString(error_list[rc*(-1)].name) + " ";
									fFail = true;
									break;
								}
								
								//free the statement
								rc = cli_free(statement);
								if(rc != cli_ok)
								{
									IT_COMMENT1("cli_free failed with code %d", rc);
									error_message += QString("cli_free failed with code ") + QString(error_list[rc*(-1)].name) + " ";
									fFail = true;
									unrecovableError = true;
									break;
								}

								rc = cli_commit(session_handle);
								if(rc != cli_ok)
								{
									IT_COMMENT1("cli_commit03 failed with code %d", rc);
									error_message += QString("cli_commit failed with code ") + QString(error_list[rc*(-1)].name) + " ";
									fFail = true;
									unrecovableError = true;
									break;
								}
							}
							else
							{
								IT_COMMENT1("cli_statement failed with code %d", statement);
								error_message += QString("cli_statement failed with code ") + QString(error_list[statement*(-1)].name) + " ";
								fFail = true;
								break;
							}
						}
					}
					break;

					case tkn_update:
					{
						if(!db_online)break;

						if (expect("table name", tkn_ident)) 
						{
							table_name = name;

							if(expect("set", tkn_set))
							{
								////////////read values////////////////
								int jk;
								int tkn, nUpdatedFields;
								tkn = 0;																
								jk = 0;

								bool loop = true;
								while (loop) 
								{
									switch (scan()) 
									{
									  case tkn_error:
									  {
											loop = false;
											break;
									  }
									  case tkn_ident:
									  {
										  strcpy(updated_field_name[jk], name);

										  if(expect("=", tkn_eq))
										  {
											 switch (scan())
											 {
									 			  case tkn_iconst:
													  strcpy(new_field_value[jk], numbuf);
													  //IT_COMMENT1("Token = %s", numbuf);
													  jk += 1;
													break;
												  case tkn_fconst:
													  strcpy(new_field_value[jk], numbuf);
													  //IT_COMMENT1("Token = %s", numbuf);
													  jk += 1;
													break;
												  case tkn_sconst:
													  strcpy(new_field_value[jk], buf);
													  //IT_COMMENT1("Token = %s", buf);
													  jk += 1;
													break;

													default:
													error("Syntax error in update");
													break;
											 }
										  }

										break;
										}																		
										default:
											error("Syntax error in update");
											loop = false;
										break;

									}

									if ((tkn = scan()) != tkn_comma) 
									{
										break;
									}
								}
								
								nUpdatedFields = jk;
								
								if(tkn == tkn_where)
								{
									query_buf = nested_select(table_name);
								}
								else
								{
									query_buf = "select * from "+ table_name;
								}
								///////////read values//////////////////
													

								//start the update!///////////////////
								
								IT_COMMENT1("Modified query: %s", (const char *)query_buf);
								statement = cli_statement(session_handle, (const char *)query_buf);							
													
								if (statement >= 0)
								{
									//update

									if(fields)
									{	
										free(fields);
										fields = NULL;
									}
										
									nCols = cli_describe(session_handle, (char const*) table_name, &fields);
									if(nCols < 0)
									{
										IT_COMMENT1("cli_describe04 failed with code %d", nCols);	
										//la riga successiva era commentata
										error_message += QString("cli_describe failed with code ") + QString(error_list[nCols*(-1)].name) + " ";
										fFail = true;
										unrecovableError = true;
										break;
									}
									else
									{
										if(nCols > max_numCol)
										{
											deallocate_bindings();
											if(allocate_bindings())
											{assert(0);break;}
										}
									}
								
									for(j = 0; j < nCols; j++)
									{
										//binding delle colonne
										switch(fields[j].type)
										{
											case cli_asciiz:
											{
												rc = cli_column(statement, fields[j].name, cli_asciiz, NULL, value_asciiz[j]);
												if(rc != cli_ok)
												{
													IT_COMMENT1("cli_column failed with code %d in cli_asciiz case", rc);
													error_message += QString("cli_column failed with code ") + QString(error_list[rc*(-1)].name) + " ";
													fFail = true;
													break;
												}
													
												
											}
											break;
											case cli_real8:
											{
												rc = cli_column(statement, fields[j].name, cli_real8, NULL, &value_real8[j]);
												if(rc != cli_ok)
												{
													IT_COMMENT1("cli_column failed with code %d in cli_real8 case", rc);
													error_message += QString("cli_column failed with code ") + QString(error_list[rc*(-1)].name) + " ";
													fFail = true;
													break;
												}
												
											}
											break;
											case cli_real4:
											{
												rc = cli_column(statement, fields[j].name, cli_real4, NULL, &value_real4[j]);
												if(rc != cli_ok)
												{
													IT_COMMENT1("cli_column failed with code %d in cli_real4 case", rc);
													error_message += QString("cli_column failed with code ") + QString(error_list[rc*(-1)].name) + " ";
													fFail = true;
													break;
												}
												
											}
											break;
											case cli_int4:
											{
												rc = cli_column(statement, fields[j].name, cli_int4, NULL, &value_int4[j]);
												if(rc != cli_ok)
												{
													IT_COMMENT1("cli_column failed with code %d in cli_int4 case", rc);
													error_message += QString("cli_column failed with code ") + QString(error_list[rc*(-1)].name) + " ";
													fFail = true;
													break;
												}
												
											}
											break;
											case cli_oid:
											{
												rc = cli_column(statement, fields[j].name, cli_oid, NULL, &value_oid[j]);
												if(rc != cli_ok)
												{
													IT_COMMENT1("cli_column failed with code %d in cli_oid case", rc);
													error_message += QString("cli_column failed with code ") + QString(error_list[rc*(-1)].name) + " ";
													fFail = true;
													break;
												}
											}
											break;
											case cli_int8:
											{
												rc = cli_column(statement, fields[j].name, cli_int8, NULL, &value_int8[j]);
												if(rc != cli_ok)
												{
													IT_COMMENT1("cli_column failed with code %d in cli_int8 case", rc);
													error_message += QString("cli_column failed with code ") + QString(error_list[rc*(-1)].name) + " ";
													fFail = true;
													break;
												}
												
											}
											break;
											default:
											unupported_types(fields[j].type);
											break;
										}
									}
									
									rc = cli_fetch(statement, cli_for_update);
														
									
									if (rc < 0) 
									{
										IT_COMMENT1("cli_fetch failed with code %d",rc);
										error_message += QString("cli_fetch failed with code ") + QString(error_list[rc*(-1)].name) + " ";
										fFail = true;
										break;
									}
								
									//Update
									
									for (k = 0;(rc = cli_get_next(statement)) == cli_ok; k++) 
									{
										//IT_COMMENT1("record # %d", k);
										
										for(j = 0; j < nCols; j++)
										{
											for(int jk = 0; jk < nUpdatedFields; jk++)
											{
												if(!strcmp(fields[j].name, updated_field_name[jk]))
												{
													switch(fields[j].type)
													{
														case cli_asciiz:
														{
															strcpy(value_asciiz[j], (const char*)new_field_value[jk]);
															jk = nUpdatedFields; //serve a terminare il ciclo
															//IT_COMMENT2("field %s cli_asciiz: %s", fields[j].name, value_asciiz[j]);
														}
														break;
														case cli_real8:
														{
															value_real8[j] = (cli_real8_t)atof((const char*)new_field_value[jk]);
															jk = nUpdatedFields; //serve a terminare il ciclo
															//IT_COMMENT2("field %s cli_real8: %lf", fields[j].name, value_real8[j]);
														}
														break;
														case cli_real4:
														{
															value_real4[j] = (cli_real4_t)atof((const char*)new_field_value[jk]);
															jk = nUpdatedFields; //serve a terminare il ciclo
															//IT_COMMENT2("field %s cli_real4: %lf", fields[j].name, value_real4[j]);
														}
														break;
														case cli_int4:
														{
															value_int4[j] = (cli_int4_t)atoi((const char*)new_field_value[jk]);
															jk = nUpdatedFields; //serve a terminare il ciclo
															//IT_COMMENT2("field %s cli_int4:  %d", fields[j].name, value_int4[j]);
														}
														break;
														case cli_oid:
														{
															value_oid[j] = (cli_oid_t)atol((const char*)new_field_value[jk]);
															jk = nUpdatedFields; //serve a terminare il ciclo
															//IT_COMMENT2("field %s cli_oid:  %d", fields[j].name, value_oid[j]);
														}
														break;
														case cli_int8:
														{
															value_int8[j] = (cli_int8_t)_atoi64((const char*)new_field_value[jk]);
															jk = nUpdatedFields; //serve a terminare il ciclo
															//IT_COMMENT2("field %s cli_int8:  %d", fields[j].name, value_int8[j]);
														}
														break;
														default:
														unupported_types(fields[j].type);
														break;
													}
												}//end if
											}
										}
									}//end for

									if(!k && (rc == cli_not_found))
									{
										IT_COMMENT("Nessun record in database");
									}
									else
									{
										rc = cli_update(statement);
										if(rc != cli_ok)  
										{
											IT_COMMENT1("cli_update failed with code %d", rc);
											error_message += QString("cli_update failed with code ") + QString(error_list[rc*(-1)].name) + " ";
											fFail = true;
											break;
										}
									}

									//free the statement
									rc = cli_free(statement);
									if(rc != cli_ok)
									{
										IT_COMMENT1("cli_free failed with code %d", rc);
										error_message += QString("cli_free failed with code ") + QString(error_list[rc*(-1)].name) + " ";
										fFail = true;
										break;
									}
									
									if(low_freq_flag || (num_recors_updated == (unsigned)commit_freq))
									{
										num_recors_updated = 0;
										
										rc = cli_commit(session_handle);
										if(rc != cli_ok)
										{
											IT_COMMENT1("cli_commit04 failed with code %d", rc);
											error_message += QString("cli_commit failed with code ") + QString(error_list[rc*(-1)].name) + " ";
											fFail = true;
											unrecovableError = true;
											break;
										}
									}
									else
									{
										num_recors_updated++;

										rc = cli_precommit(session_handle);
										if(rc != cli_ok)  
										{
											IT_COMMENT1("cli_precommit failed with code %d", rc);
											error_message += QString("cli_precommit failed with code ") + QString(error_list[rc*(-1)].name) + " ";
											fFail = true;
											break;
										}
									}
								}
								else
								{
									IT_COMMENT1("cli_statement failed with code %d", statement);
									error_message += QString("cli_statement failed with code ") + QString(error_list[statement*(-1)].name) + " ";
									fFail = true;
									break;
								}

								////end update///////////////////////
							}
						}
					}
					break;

					case tkn_create:
					{
						if(!db_online)break;

						rc = cli_create_table_apa(session_handle, giga_query);
						if(rc != cli_ok)
						{
							IT_COMMENT1("cli_create failed with code %d", rc);
							error_message += QString("cli_create failed with code ") + QString(error_list[rc*(-1)].name) + " ";
							fFail = true;
							break;
						}

						rc = cli_commit(session_handle);
						if(rc != cli_ok)  
						{
							IT_COMMENT1("cli_commit05 failed with code %d", rc);
							error_message += QString("cli_commit failed with code ") + QString(error_list[rc*(-1)].name) + " ";
							fFail = true;
							unrecovableError = true;
							break;
						}

						deallocate_bindings();
						if(allocate_bindings())
						{assert(0);break;}
					}
					break;
					/*
					case tkn_create:
					{
						if(!db_online)break;
						
						switch (scan()) 
						{
							case tkn_hash:
							case tkn_index:
							{
								if (expect("on", tkn_on) && expect("table name", tkn_ident)) 
								{
								   table_name = name;
	   							   if (expect(".", tkn_dot) && expect("field name", tkn_ident)) 
								   {
									    QString fieldName = name;

										rc = cli_alter_index(session_handle, (char const*) table_name, (char const*)fieldName, cli_indexed);
										if(rc != cli_ok)
										{
											IT_COMMENT1("cli_alter_index failed with code %d", rc);
											error_message += QString(" cli_alter_index failed with code ") + QString(error_list[rc*(-1)].name) + " ";
											fFail = true;
											break;
										}

										rc = cli_commit(session_handle);
										if(rc != cli_ok)
										{
											IT_COMMENT1("cli_commit failed with code %d", rc);
											error_message += QString(" cli_commit failed with code ") + QString(error_list[rc*(-1)].name) + " ";
											fFail = true;
											unrecovableError = true;
											break;
										}

										deallocate_bindings();
										if(allocate_bindings())
										{break;}
								   }
								}
							}
							break;
							case tkn_table:
							{
								if(expect("table name", tkn_ident)) 
								{
								    table_name = name;
																		
									if(!expect("(", tkn_lpar))
									{ 
										fFail = true;
										break;
									}

									const int maxFields = NMAX_FIELDS_IN_TABLE;

									if(fields)
									{	
										free(fields);
										fields = NULL;
									}

									fields = (cli_field_descriptor*)malloc(maxFields*sizeof(cli_field_descriptor));
									
									int nFields = 0;
									int nColumns = 0;
									enum cli_var_type type;
									int tkn = tkn_comma;
									
									while (tkn == tkn_comma) 
									{
										if (nFields+1 == maxFields) 
										{
											IT_COMMENT(" Too many fields");
											error_message += QString(" Too many fields");
											fFail = true;
											break;
										}

										if(!expect("field name", tkn_ident))
										{
											fFail = true;
											break;
										}
																		
										int nameLen = strlen(name)+1;
										fields[nFields].name = new char[nameLen];
										strcpy(fields[nFields].name, name);
										fields[nFields].refTableName = NULL;
										fields[nFields].inverseRefFieldName = NULL;
										fields[nFields].flags = 0; //no index
																						
										switch (scan()) 
										{
											case tkn_bool:
											type =  cli_bool;
											break;
											case tkn_int1:
											type =  cli_int1;
											break;
											case tkn_int2:
											type =  cli_int2;
											break;
											case tkn_int4:
											type =  cli_int4;
											break;
											case tkn_int8:
											type =  cli_int8;
											break;
											case tkn_real4:
											type =  cli_real4;
											break;
											case tkn_real8:
											type =  cli_real8;
											break;
											case tkn_string:
											type =  cli_asciiz;
											break;
											case tkn_reference:
											{
												if(expect("to", tkn_to) && expect("referenced table name", tkn_ident)) 
												{
													fields[nFields].refTableName = new char[STRLEN(name)+1];
													strcpy(fields[nFields].refTableName, name);
													type = cli_oid;
													break;
												}
												else
												{ 
													type = cli_unknown;
													break;
												}
											}
										    break;
											default:
											{
												type = cli_unknown;
												IT_COMMENT(" 'field type' expected");
												error_message += QString(" 'field type' expected");
												fFail = true;
												break;
											}
										}

										fields[nFields].type = type;
																			
										if(unupported_types(fields[nFields].type)) 
										{
											fFail = true;
											break;
										}

										nFields += 1;
										nColumns += 1;
										tkn = scan();
									}

									if (tkn == tkn_rpar) 
									{
										rc = cli_create_table(session_handle, (char const*) table_name, nColumns, fields);
	   									if(rc != cli_ok)
										{
											IT_COMMENT1(" cli_create_table failed with code %d", rc);
											error_message += QString(" cli_create_table failed with code ") + QString(error_list[rc*(-1)].name) + " ";
											fFail = true;
											break;
										}

										rc = cli_commit(session_handle);
										if(rc != cli_ok)  
										{
											IT_COMMENT1(" cli_commit failed with code %d", rc);
											error_message += QString(" cli_commit failed with code ") + QString(error_list[rc*(-1)].name) + " ";
											fFail = true;
											unrecovableError = true;
											break;
										}

										deallocate_bindings();
										if(allocate_bindings())
										{break;}
									}
									else
									{
										IT_COMMENT(" ')' expected");
										error_message += QString(" ')' expected");
										fFail = true;
										break;
									}
								}
							}
							break;
							default:
							{
								IT_COMMENT(" Expecting 'table', 'hash' or 'index' keyword");
								error_message += QString(" Expecting 'table', 'hash' or 'index' keyword");
								fFail = true;
								break;
							}
						}//end switch (scan())
					}
					break;
					*/
					case tkn_alter:
					{
						if(!db_online)break;
						
						if(expect("table", tkn_table) && expect("table name", tkn_ident)) 
						{
							table_name = name;
																
							if(!expect("(", tkn_lpar))
							{ 
								fFail = true;
								break;
							}

							const int maxFields = NMAX_FIELDS_IN_TABLE;

							if(fields)
							{	
								free(fields);
								fields = NULL;
							}

							fields = (cli_field_descriptor*)malloc(maxFields*sizeof(cli_field_descriptor));
							
							int nFields = 0;
							int nColumns = 0;
							enum cli_var_type type;
							int tkn = tkn_comma;
							
							while (tkn == tkn_comma) 
							{
								if (nFields+1 == maxFields) 
								{
									IT_COMMENT(" Too many fields");
									error_message += QString(" Too many fields");
									fFail = true;
									break;
								}

								if(!expect("field name", tkn_ident))
								{
									fFail = true;
									break;
								}
																
								int nameLen = strlen(name)+1;
								fields[nFields].name = new char[nameLen];
								strcpy(fields[nFields].name, name);
								fields[nFields].refTableName = NULL;
								fields[nFields].inverseRefFieldName = NULL;
								fields[nFields].flags = 0; //no index
																				
								switch (scan()) 
								{
									case tkn_bool:
									type =  cli_bool;
									break;
									case tkn_int1:
									type =  cli_int1;
									break;
									case tkn_int2:
									type =  cli_int2;
									break;
									case tkn_int4:
									type =  cli_int4;
									break;
									case tkn_int8:
									type =  cli_int8;
									break;
									case tkn_real4:
									type =  cli_real4;
									break;
									case tkn_real8:
									type =  cli_real8;
									break;
									case tkn_string:
									type =  cli_asciiz;
									break;
									case tkn_reference:
									{
										if(expect("to", tkn_to) && expect("referenced table name", tkn_ident)) 
										{
											fields[nFields].refTableName = new char[STRLEN(name)+1];
											strcpy(fields[nFields].refTableName, name);
											/*
											int tkn = scan();
											if(tkn == tkn_inverse) 
											{
												if (!expect("inverse reference field name", tkn_ident)) 
												{ 
													type = cli_unknown;
													break;
												}
												fields[nFields].inverseRefFieldName = new char[STRLEN(name)+1];
												strcpy(fields[nFields].inverseRefFieldName, name);
											} 
											else 
											{ 
												inverseRefName = NULL;
												ungetToken = tkn;
											}
											*/
											type = cli_oid;
											break;
										}
										else
										{ 
											type = cli_unknown;
											break;
										}
									}
									break;
									default:
									{
										type = cli_unknown;
										IT_COMMENT(" 'field type' expected");
										error_message += QString(" 'field type' expected");
										fFail = true;
										break;
									}
								}
																
								fields[nFields].type = type;
								
								if(unupported_types(fields[nFields].type)) 
								{
									fFail = true;
									break;
								}

								nFields  += 1;
								nColumns += 1;
								tkn = scan();
							}

							if (tkn == tkn_rpar) 
							{
								rc = cli_alter_table(session_handle, (char const*) table_name, nColumns, fields);
 								if(rc != cli_ok)
								{
									IT_COMMENT1(" cli_alter_table failed with code %d", rc);
									error_message += QString(" cli_alter_table failed with code ") + QString(error_list[rc*(-1)].name) + " ";
									fFail = true;
									break;
								}

								rc = cli_commit(session_handle);
								if(rc != cli_ok)  
								{
									IT_COMMENT1(" cli_commit06 failed with code %d", rc);
									error_message += QString(" cli_commit failed with code ") + QString(error_list[rc*(-1)].name) + " ";
									fFail = true;
									unrecovableError = true;
									break;
								}

								deallocate_bindings();
								if(allocate_bindings())
								{assert(0);break;}
							}
							else
							{
								IT_COMMENT(" ')' expected");
								error_message += QString(" ')' expected");
								fFail = true;
								break;
							}
						}
					}
					break;

					case tkn_drop:
					{
						if(!db_online)break;

						switch (scan())
						{
							case tkn_hash:
							case tkn_index:
							{
								if (expect("on", tkn_on) && expect("table name", tkn_ident)) 
								{
								   table_name = name;
	   							   if (expect(".", tkn_dot) && expect("field name", tkn_ident)) 
								   {
									    QString fieldName = name;

										rc = cli_alter_index(session_handle, (char const*) table_name, (char const*)fieldName, 0);
										if(rc != cli_ok)
										{
											IT_COMMENT1("cli_alter_index failed with code %d", rc);
											error_message += QString("cli_alter_index failed with code ") + QString(error_list[rc*(-1)].name) + " ";
											fFail = true;
											break;
										}

										rc = cli_commit(session_handle);
										if(rc != cli_ok)  
										{
											IT_COMMENT1("cli_commit07 failed with code %d", rc);
											error_message += QString("cli_commit failed with code ") + QString(error_list[rc*(-1)].name) + " ";
											fFail = true;
											unrecovableError = true;
											break;
										}

										deallocate_bindings();
										if(allocate_bindings())
										{assert(0);break;}
								   }
								}
							}
							break;
							case tkn_table:
							{
								if(expect("table name", tkn_ident)) 
								{
									table_name = name;

									rc = cli_drop_table(session_handle, (char const*) table_name);
									if(rc != cli_ok)
									{
										IT_COMMENT1("cli_drop_table failed with code %d", rc);
										error_message += QString(" cli_drop_table failed with code ") + QString(error_list[rc*(-1)].name) + " ";
										fFail = true;
										break;
									}

									rc = cli_commit(session_handle);
									if(rc != cli_ok)  
									{
										IT_COMMENT1("cli_commit08 failed with code %d", rc);
										error_message += QString(" cli_commit failed with code ") + QString(error_list[rc*(-1)].name) + " ";
										fFail = true;
										unrecovableError = true;
										break;
									}

									deallocate_bindings();
									if(allocate_bindings())
									{assert(0);break;}
								}
							}
							break;
							default:
							{
								IT_COMMENT(" expecting 'table', 'hash' or 'index' keyword");
								error_message += QString(" expecting 'table', 'hash' or 'index' keyword");
								fFail = true;
								break;
							}
						}//end switch (scan())
					}
					break;

					case tkn_backup:
					{
						if (!db_online) 
						{
							error("Database not opened");
							break;
						}

						if (expect("backup file name", tkn_sconst)) 
						{
							rc = cli_backup(session_handle, buf);
							if(rc != cli_ok)
							{
							   IT_COMMENT1("cli_backup failed with code %d", rc);
							   error_message += QString("cli_backup failed with code ") + QString(error_list[rc*(-1)].name) + " ";
							   fFail = true;
							   break;
							} 
							else 
							{
								rc = cli_commit(session_handle);
								if(rc != cli_ok)  
								{
									IT_COMMENT1("cli_commit09 failed with code %d", rc);
									error_message += QString("cli_commit failed with code ") + QString(error_list[rc*(-1)].name) + " ";
									fFail = true;
									unrecovableError = true;
									break;
								}
							}
						}
					}
					break;
					case tkn_restore:
					{
						if (db_online) 
						{
							error("Can not restore online database");
							break;
						}

						if (expect("backup file name", tkn_sconst)) 
						{
							char backup_file[MAX_PATH];

							strcpy(backup_file, buf);
														
							if (expect("database file name", tkn_sconst)) 
							{
								char db_file[MAX_PATH];
								strcpy(db_file, buf);

								rc = cli_restore(session_handle, backup_file, db_file);
								if(rc != cli_ok)
								{
								   IT_COMMENT1("cli_restore failed with code %d", rc);
								   error_message += QString("cli_restore failed with code ") + QString(error_list[rc*(-1)].name) + " ";
								   fFail = true;
								   break;
								} 
							}
						}
					}
					break;
					case tkn_open:
					{
						if (expect("database file name", tkn_sconst)) 
						{
							if (db_online) 
							{
								rc = put_db_offline(session_handle);
								if(rc != cli_ok)
								{
									IT_COMMENT1("put_db_offline failed with code %d", rc);
									error_message += QString("put_db_offline failed with code ") + QString(error_list[rc*(-1)].name) + " ";
									fFail = true;
									break;
								}
								else
								{
									db_online = false;
								}
							}

							rc = put_db_online(session_handle, buf);
							if(rc != cli_ok)
							{
								IT_COMMENT1("put_db_online failed with code %d", rc);
								error_message += QString("put_db_online failed with code ") + QString(error_list[rc*(-1)].name) + " ";
								fFail = true;
								break;
							}
							else
							{
								db_online = true;

								deallocate_bindings();
								if(allocate_bindings())
								{assert(0);break;}
							}
						}
					}
					break;
					case tkn_close:
					{
						if(!db_online)break;

						rc = put_db_offline(session_handle);
						if(rc != cli_ok)
						{
							IT_COMMENT1("put_db_offline failed with code %d", rc);
							error_message += QString("put_db_offline failed with code ") + QString(error_list[rc*(-1)].name) + " ";
							fFail = true;
							break;
						}
						else
						{
							db_online = false;
						}
					}
					break;
					case tkn_insert_blob: //insertblob
					{
						if(!db_online)break;

						if(pLastResult) assert(0);
						pLastResult = new Giga_Result;
						pLastResult->Tuples.resize(limit_of_records);

						pLastResult->fieldNames << QString("BLOB_ID");

						QStringList *pT;
						
						//Formato query:
						//insertblob <blob.........\0>

						//N.B. In this particular implementation the blob is
						//a very long string terminated with \0 so I can determine its
						//length with strlen
																		
						char* blob = giga_query + pos + 1;

						cli_oid_t blob_id = 0;
						
						rc = cli_create_blob_item(session_handle, blob, strlen(blob) + 1, &blob_id);
						if(rc != cli_ok)
						{
							IT_COMMENT1("cli_create_blob_item failed with code %d", rc);
							error_message += QString("cli_create_blob_item failed with code ") + QString(error_list[rc*(-1)].name) + " ";
							fFail = true;
							break;
						}
						else
						{
							pT = new QStringList;
							pLastResult->Tuples.insert(0, pT);
							*pT << QString::number(blob_id);
						}

						rc = cli_commit(session_handle);
						if(rc != cli_ok)  
						{
							IT_COMMENT1("cli_commit10 failed with code %d", rc);
							error_message += QString("cli_commit failed with code ") + QString(error_list[rc*(-1)].name) + " ";
							fFail = true;
							unrecovableError = true;
							break;
						}
					}
					break;
					case tkn_get_blob: //getblob
					{
						if(!db_online)break;
						//Formato query:
						//getblob <blobid> 
						//blobid is returned by cli_create_blob_item

						//WARNING: if blobid does not referece to an existing blob into the db,
						//an unpredictable behavior of the databases is garanteed

						if(pLastResult) assert(0);
						pLastResult = new Giga_Result;
						pLastResult->Tuples.resize(limit_of_records);

						pLastResult->fieldNames << QString("BLOB");

						QStringList *pT;
						
						int tk = scan();

						if(tk == tkn_iconst)
						{
							unsigned char* blob = 0;
								
							rc = cli_get_blob_item(session_handle, ival, &blob);																					
							if(rc != cli_ok)
							{
								//if(rc == cli_not_found)
								//{
								//	db_message += QString(" Blob not found ") + QString("blobname");
								//	error_message += QString("Blob not found ") + QString("blobname");
								//	fFail = true;
								//}
								//else
								//{
									IT_COMMENT1("cli_get_blob_item failed with code %d", rc);
									error_message += QString("cli_get_blob_item failed with code ") + QString(error_list[rc*(-1)].name) + " ";
									fFail = true;
									break;
								//}
							}
							else
							{
								pT = new QStringList;
								pLastResult->Tuples.insert(0, pT);
								*pT << QString((char*)blob);
								free(blob);
							}
						}
						
						rc = cli_commit(session_handle);
						if(rc != cli_ok)  
						{
							IT_COMMENT1("cli_commit11 failed with code %d", rc);
							error_message += QString("cli_commit failed with code ") + QString(error_list[rc*(-1)].name) + " ";
							fFail = true;
							unrecovableError = true;
							break;
						}
					}
					break;
					case tkn_delete_blob:
					{
						if(!db_online)break;

						//Formato query:
						//deleteblob <blobid> 
						//blobid is returned by cli_create_blob_item

						//WARNING: if blobid does not referece to an existing blob into the db,
						//an unpredictable behavior of the databases is garanteed
						
						int tk = scan();

						if(tk == tkn_iconst)
						{
							rc = cli_delete_blob_item(session_handle, ival);
							if(rc != cli_ok)
							{
								//if(rc == cli_not_found)
								//{
								//	db_message += QString("Blob not found ") + QString("blobname");
								//	error_message += QString("Blob not found ") + QString("blobname");
								//	fFail = true;
								//}
								//else
								//{
									IT_COMMENT1("cli_delete_blob_item failed with code %d", rc);
									error_message += QString("cli_delete_blob_item failed with code ") + QString(error_list[rc*(-1)].name) + " ";
									fFail = true;
									break;
								//}
							}
						}

						rc = cli_commit(session_handle);
						if(rc != cli_ok)  
						{
							IT_COMMENT1("cli_commit12 failed with code %d", rc);
							error_message += QString("cli_commit failed with code ") + QString(error_list[rc*(-1)].name) + " ";
							fFail = true;
							unrecovableError = true;
							break;
						}
					}
					break;
					case tkn_rollback:
					{
						rc = cli_abort(session_handle);
						if(rc != cli_ok)  
						{
							IT_COMMENT1("cli_abort failed with code %d", rc);
							error_message += QString("cli_abort failed with code ") + QString(error_list[rc*(-1)].name) + " ";
							fFail = true;
							break;
						}
					}
					break;
					case tkn_show:
					{
						if(tables)
						{
							free(tables);
							tables = NULL;
						}

						if(pLastResult) assert(0);
						pLastResult = new Giga_Result;
						pLastResult->Tuples.resize(limit_of_records);

						pLastResult->fieldNames << QString("TABLE_NAME");

						QStringList *pT;

						unsigned int ntables = 0;
						ntables = cli_show_tables(session_handle, &tables);
						if(ntables < 0)
						{
							IT_COMMENT1("cli_show_tables failed with code %d", ntables);
							error_message += QString("cli_show_tables failed with code ") + QString(error_list[ntables*(-1)].name) + " ";
							fFail = true;
							break;
						}
						else if((ntables > 0) && tables)
						{
							for(unsigned int i = 0; i < ntables; i++)
							{
								pT = new QStringList;
								pLastResult->Tuples.insert(i, pT);
								*pT << QString(tables[i].name);
							}
						}
						else
						{
							pT = new QStringList;
							pLastResult->Tuples.insert(0, pT);
							*pT << QString(DB_NOT_CONNECTED);
						}
					}
					break;
					case tkn_describe:
					{
						if(pLastResult) assert(0);
						pLastResult = new Giga_Result;
						pLastResult->Tuples.resize(limit_of_records);

						pLastResult->fieldNames << QString("FIELD_NAME");
						pLastResult->fieldNames << QString("FIELD_TYPE");
						pLastResult->fieldNames << QString("FIELD_INDEX");

						QStringList *pT;
						
						int tk = scan();

						if(tk == tkn_ident)
						{
							if(fields)
							{	
								free(fields);
								fields = NULL;
							}
							//BUG: se la tabella non esiste => la cli_describe ha comportmento imprevedibile;
							
							unsigned int nfields = 0;
							nfields = cli_describe(session_handle, buf, &fields);
							if(nfields < 0)
							{
								IT_COMMENT1("cli_describe05 failed with code %d", nfields);	
								//la riga successiva era commentata
								error_message += QString("cli_describe failed with code ") + QString(error_list[nfields*(-1)].name) + " ";
								fFail = true;
								unrecovableError = true;
								break;
							}
							else
							{
								for(unsigned int i = 0; i < nfields; i++)
								{
									pT = new QStringList;
									pLastResult->Tuples.insert(i, pT);
									*pT << QString(fields[i].name);
									*pT << QString(var_type_list[fields[i].type].name);
									*pT << QString(index_type_list[fields[i].flags].name);
								}
							}
						}
					}
					break;
					default:
					{
					  error("Unexpected token");
					}
				}// end switch(main_token) 
			}//
			//
			if(error_message == QString::null)
			{
				db_message += QString(QObject::tr(" Result ok "));
			}
			IT_COMMENT("Transaction Done");

			// tell the parent that the transaction has completed
			Done = true;
			if(Parent) QThread::postEvent(Parent,new QEvent(QEvent::User)); 
			//
		}// end if(Trigger)
		#ifdef WIN32
		else
		{
			QThread::msleep(10);		
		}	
		#endif
	}//end while(true)
}


QString Giga_Thread::nested_select(const QString &table_name)
{
	IT_IT("Giga_Thread::nested_select");

	QString condition, query_buf;

	int main_token;
	bool star;

	if (readCondition()) 
	{
		QString new_condition;
		QString new_where = " where";
		new_condition = QString(" where") + buf;
										
		strcpy(giga_query,(const char*) new_condition);
		pos = 0;
		
		int token = scan();//read tkn_where
		
		for(token = scan(); token != tkn_where ; token = scan())
		{
			switch(token)
			{
				case tkn_sconst:
					new_where += QString("\'") + buf + QString("\'");
				break;
				case tkn_eq:
					new_where += "=";
				break;
				case tkn_and:
					new_where += " and";
				break;
				case tkn_or:
					new_where += " or";
				break;
				case tkn_ident:
					new_where += QString(" ") + name;
				break;
				case tkn_in:
				{
					new_where += " in";		
					goto exit_loop_in;
				}

				default:
				break;
			}
		}
		exit_loop_in:

		if(token == tkn_in)
		{
			for(token = scan(); token != tkn_lpar ; token = scan())
			{
				;
			}
			//bug bug: no ')' are allowed in NESTED query!
			int i, ch;
			for (i = 0; (ch = get()) && ch !=  ')'; i++) 
			{
				buf[i] = ch;
			}

			buf[i] = '\0';
														
			//////execute second query///////////////////
			IT_COMMENT1("NESTED QUERY: %s", buf);
			strcpy(giga_query, buf);
			pos = 0;

			main_token = scan();

			if(main_token == tkn_select)
			{
				int nFieldsSelected2;
				int tk2;
				tk2 = 0;
				
				bool loop = true;
				while(loop)
				{
					switch(scan())
					{
						case tkn_all:
						star = true;
						break;
						case tkn_from:
						loop = false;
						break;
						case tkn_ident:
						{
							//I get a field name
							//IT_COMMENT1("seltoken = %s", buf);
							strcpy(selected_field_name2[tk2], buf);
							tk2 += 1;
						}
						break;
						case tkn_comma:
							//IT_COMMENT("seltoken = ','");
						break;
						default:
							error("Error in select scan");
							//IT_COMMENT1("seltoken = %s", buf);
						break;
					}
				}
				
				nFieldsSelected2 = tk2;
				
				if (scan() != tkn_ident) 
				{
					error("Table name expected");
				}
				
				QString table_name2 = name;

				if (readCondition()) 
				{
					condition = buf;
				}
				else
				{
					condition = "";
				}

				query_buf = "select * from "+ table_name2 + condition;
				QString query_result = "";
				int first_item = 0;

				IT_COMMENT1("Modified second query: %s", (const char *)query_buf);
				statement = cli_statement(session_handle, (const char *)query_buf);		
				
				if (statement >= 0)
				{
					//NESTED query select

					if(fields)
					{	
						free(fields);
						fields = NULL;
					}
						
					nCols = cli_describe(session_handle, (char const*) (table_name2), &fields);
					if(nCols < 0)
					{
						IT_COMMENT1("cli_describe06 failed with code %d", nCols);	
						//la riga successiva era commentata
						error_message += QString("cli_describe failed with code ") + QString(error_list[nCols*(-1)].name) + " ";
						fFail = true;
						unrecovableError = true;
						QString a = "";
						return a;
					}
					else
					{
						if(nCols > max_numCol)
						{
							deallocate_bindings();
							if(allocate_bindings())
							{
								assert(0);
								QString a = "";
								return a;
							}
						}
					}
					
					//NESTED query binding delle colonne
					for(int j = 0; j < nCols; j++)
					{
						switch(fields[j].type)
						{
							case cli_asciiz:
							{
								rc = cli_column(statement, fields[j].name, cli_asciiz, NULL, value_asciiz[j]);
								if(rc != cli_ok)
								{
									IT_COMMENT1("cli_column failed with code %d in cli_asciiz case", rc);
									error_message += QString("cli_column failed with code ") + QString(error_list[rc*(-1)].name) + " ";
									fFail = true;
									break;
								}
							}
							break;
							case cli_real8:
							{
								rc = cli_column(statement, fields[j].name, cli_real8, NULL, &value_real8[j]);
								if(rc != cli_ok)
								{
									IT_COMMENT1("cli_column failed with code %d in cli_real8 case", rc);
									error_message += QString("cli_column failed with code ") + QString(error_list[rc*(-1)].name) + " ";
									fFail = true;
									break;
								}
							}
							break;
							case cli_real4:
							{
								rc = cli_column(statement, fields[j].name, cli_real4, NULL, &value_real4[j]);
								if(rc != cli_ok)
								{
									IT_COMMENT1("cli_column failed with code %d in cli_real4 case", rc);
									error_message += QString("cli_column failed with code ") + QString(error_list[rc*(-1)].name) + " ";
									fFail = true;
									break;
								}
							}
							break;
							case cli_int4:
							{
								rc = cli_column(statement, fields[j].name, cli_int4, NULL, &value_int4[j]);
								if(rc != cli_ok)
								{
									IT_COMMENT1("cli_column failed with code %d in cli_int4 case", rc);
									error_message += QString("cli_column failed with code ") + QString(error_list[rc*(-1)].name) + " ";
									fFail = true;
									break;
								}
							}
							break;
							case cli_oid:
							{
								rc = cli_column(statement, fields[j].name, cli_oid, NULL, &value_oid[j]);
								if(rc != cli_ok)
								{
									IT_COMMENT1("cli_column failed with code %d in cli_oid case", rc);
									error_message += QString("cli_column failed with code ") + QString(error_list[rc*(-1)].name) + " ";
									fFail = true;
									break;
								}
							}
							break;
							case cli_int8:
							{
								rc = cli_column(statement, fields[j].name, cli_int8, NULL, &value_int8[j]);
								if(rc != cli_ok)
								{
									IT_COMMENT1("cli_column failed with code %d in cli_int8 case", rc);
									error_message += QString("cli_column failed with code ") + QString(error_list[rc*(-1)].name) + " ";
									fFail = true;
									break;
								}
							}
							break;
							default:
							unupported_types(fields[j].type);
							break;
						}
					}

					rc = cli_fetch(statement, cli_view_only);
					if (rc < 0) 
					{
						IT_COMMENT1("cli_fetch failed with code %d",rc);
						error_message += QString("cli_fetch failed with code ") + QString(error_list[rc*(-1)].name) + " ";
						fFail = true;
						QString a = "";
						return a;
					}
					
					//FETCH OF NESTED QUERY
																	
					for (int k = 0;((rc = cli_get_next(statement)) == cli_ok); k++) 
					{
						//IT_COMMENT1("record # %d", k);
																			
						for(j = 0; j < nCols; j++)
						{
							if(nFieldsSelected2)
							{
								for(int tk2 = 0; tk2 < nFieldsSelected2; tk2++)
								{
									if(!strcmp(fields[j].name, selected_field_name2[tk2]))
									{
																						
										switch(fields[j].type)
										{
											case cli_asciiz:
											{
												if(first_item){query_result += ",";} 
												query_result += "\'" + QString(value_asciiz[j]) + "\'";
												first_item = 1;
												//IT_COMMENT2("field %s cli_asciiz: %s", fields[j].name, value_asciiz[j]);
												tk2 = nFieldsSelected2; //serve a finire il ciclo!
											}
											break;
											case cli_real8:
											{
												if(first_item){query_result += ",";} 
												query_result += QString::number(((cli_real8_t)	value_real8[j]));
												first_item = 1;
												//IT_COMMENT2("field %s cli_real8: %lf", fields[j].name, value_real8[j]);
												tk2 = nFieldsSelected2; //serve a finire il ciclo!
											}
											break;
											case cli_real4:
											{
												if(first_item){query_result += ",";} 
												query_result += QString::number(((cli_real4_t)	value_real4[j]));
												first_item = 1;
												//IT_COMMENT2("field %s cli_real4: %lf", fields[j].name, value_real4[j]);
												tk2 = nFieldsSelected2; //serve a finire il ciclo!
											}
											break;
											case cli_int4:
											{
												if(first_item){query_result += ",";} 
												query_result += QString::number(((cli_int4_t)	value_int4[j]));
												first_item = 1;
												//IT_COMMENT2("field %s cli_int4:  %d", fields[j].name, value_int4[j]);
												tk2 = nFieldsSelected2; //serve a finire il ciclo!
											}
											break;
											case cli_oid:
											{
												if(first_item){query_result += ",";} 
												query_result += QString::number(((cli_oid_t)	value_oid[j]));
												first_item = 1;
												//IT_COMMENT2("field %s cli_oid:  %d", fields[j].name, value_oid[j]);
												tk2 = nFieldsSelected2; //serve a finire il ciclo!
											}
											break;
											case cli_int8:
											{
												if(first_item){query_result += ",";} 
																								
												char buffer[20];
												_i64toa(value_int8[j], buffer, 10);
												query_result += QString(buffer);

												first_item = 1;

												//IT_COMMENT2("field %s cli_int8:  %d", fields[j].name, value_int8[j]);
												tk2 = nFieldsSelected2; //serve a finire il ciclo!
											}
											break;
											default:
											unupported_types(fields[j].type);
											break;
										}
									}
								}
							}
							else
							{
								//NESTED query select *

								switch(fields[j].type)
								{
									case cli_asciiz:
									{
										if(first_item){query_result += ",";} 
										query_result += "\'"+ QString(value_asciiz[j]) + "\'";
										first_item = 1;
										//IT_COMMENT2("field %s cli_asciiz: %s", fields[j].name, value_asciiz[j]);
									}
									break;
									case cli_real8:
									{
										if(first_item){query_result += ",";} 
										query_result += QString::number(((cli_real8_t)	value_real8[j]));
										first_item = 1;
										//IT_COMMENT2("field %s cli_real8: %lf", fields[j].name, value_real8[j]);
									}
									break;
									case cli_real4:
									{
										if(first_item){query_result += ",";} 
										query_result += QString::number(((cli_real4_t)	value_real4[j]));
										first_item = 1;
										//IT_COMMENT2("field %s cli_real4: %lf", fields[j].name, value_real4[j]);
									}
									break;
									case cli_int4:
									{
										if(first_item){query_result += ",";} 
										query_result += QString::number(((cli_int4_t)	value_int4[j]));
										first_item = 1;
										//IT_COMMENT2("field %s cli_int4:  %d", fields[j].name, value_int4[j]);
									}
									break;
									case cli_oid:
									{
										if(first_item){query_result += ",";} 
										query_result += QString::number(((cli_oid_t)	value_oid[j]));
										first_item = 1;
										//IT_COMMENT2("field %s cli_oid:  %d", fields[j].name, value_oid[j]);
									}
									break;
									case cli_int8:
									{
										if(first_item){query_result += ",";} 

										char buffer[20];
										_i64toa(value_int8[j], buffer, 10);
										query_result += QString(buffer);

										first_item = 1;
										//IT_COMMENT2("field %s cli_int8:  %d", fields[j].name, value_int8[j]);
									}
									break;
									default:
									unupported_types(fields[j].type);
									break;
								}
								//end select *
							} //end else
						}
						
					}//end for
					
					if(!k && (rc == cli_not_found)){IT_COMMENT("Nessun record in database");}

					//free the statement
					rc = cli_free(statement);
					if(rc != cli_ok)
					{
						IT_COMMENT1("cli_free failed with code %d", rc);
						error_message += QString("cli_free failed with code ") + QString(error_list[rc*(-1)].name) + " ";
						fFail = true;
						QString a = "";
						return a;
					}

					rc = cli_commit(session_handle);
					if(rc != cli_ok)  
					{
						IT_COMMENT1("cli_commit13 failed with code %d", rc);
						error_message += QString("cli_commit failed with code ") + QString(error_list[rc*(-1)].name) + " ";
						fFail = true;
						unrecovableError = true;
						QString a = "";
						return a;
					}
				}
				else
				{
					IT_COMMENT1("cli_statement failed with code %d", statement);
					error_message += QString("cli_statement failed with code ") + QString(error_list[statement*(-1)].name) + " ";
					fFail = true;
					QString a = "";
					return a;
				}

				if(query_result != "")
				{
					condition = new_where + "("+ query_result + ")";
				}
				else
				{
					condition = new_where + "('')";
				}

			}//if(main_token == tkn_select)
			else
			{
				condition = new_condition;
			}
		}
		else
		{
			condition = new_condition;
		}
	}
	else
	{
		condition = "";
	}

	query_buf = "select * from "+ table_name + condition;

	return query_buf;
}


int Giga_Thread::get()
{
	int ch;
		
	ch = giga_query[pos];

    if (ch == '\0') 
	{
		pos = 0;
    } 
	else if	(ch == '\t') 
	{
		pos = DOALIGN(pos + 1, 8);
    } 
	else 
	{
		pos += 1;
    }
    return ch;
}


void Giga_Thread::unget(int ch) {
    
	if (ch != '\n') 
	{
		pos -= 1;
	} 
}


void Giga_Thread::error(char const* msg)
{
	IT_IT("Giga_Thread::error");
	IT_COMMENT(msg);
	fFail = true;
	error_message += QString("Error -> [") + QString(msg) + QString("] in query ->") + QString(giga_query);
}

int Giga_Thread::scan()
{
	int i, ch, digits;
	
	do 
	{	if ((ch = get()) == '\0') 
		{
			return tkn_null;
		}
    } while (ch > 0 && ch <= ' ');

    tknPos = pos;
    switch (ch) {
		  case '*':
		return tkn_all;
		  case '=':
		return tkn_eq;
		  case '(':
		return tkn_lpar;
		  case ')':
		return tkn_rpar;
		  case ',':
		return tkn_comma;
		  case '.':
		return tkn_dot;
		  case ';':
		return tkn_semi;
		  case '\'':
		i = 0;
		while (true) {
			ch = get();
			if (ch == '\'') 
			{
				if ((ch = get()) != '\'') 
				{
					unget(ch);
					break;
				}
			} 
			else if (ch == '\n' || ch == '\0') 
			{
				unget(ch);
				error("New line within character constant");
				return tkn_error;
			}

			if (i+1 == buflen) 
			{
				char* newbuf = new char[buflen*2];
				memcpy(newbuf, buf, buflen*sizeof(char));
				delete[] buf;
				buf = newbuf;
				buflen *= 2;
			}
			buf[i++] = ch;
		}
		buf[i] = '\0';
		return tkn_sconst;
		  case '-':
		  case '0': case '1': case '2': case '3': case '4':
		  case '5': case '6': case '7': case '8': case '9':
		  case '+':
		i = 0;
		do {
			numbuf[i++] = (char)ch;
			if (i == MAX_LENGHT_OF_STRING) 
			{
				error("Numeric constant too long");
				return tkn_error;
			}
			ch = get();
		} while (ch != '\0' && ((ch >= '0' && ch <= '9') || ch == '+' || ch == '-' || ch == 'e' || ch == 'E' || ch == '.'));
		unget(ch);
		numbuf[i] = '\0';
		if (sscanf(numbuf, INT8_FORMAT "%n", &ival, &digits) != 1) 
		{
			error("Bad integer constant");
			return tkn_error;
		}
		if (digits != i) 
		{
			if (sscanf(numbuf, "%lf%n", &fval, &digits) != 1 || digits != i) {
			error("Bad float constant");
			return tkn_error;
			}
			return tkn_fconst;
		}
		return tkn_iconst;

		  case '<':
			if ((ch = get()) == '=') 
			{
				return tkn_le;
			} 
			else if (ch == '>') 
			{
				return tkn_ne;
			}
			unget(ch);
			return tkn_lt;
		  case '>':
			if ((ch = get()) == '=') 
			{
				return tkn_ge;
			}
			unget(ch);
			return tkn_gt;

		default:
		if (ISALNUM(ch) || ch == '$' || ch == '_') 
		{
			i = 0;
			do {
				buf[i++] = ch;
				if (i == buflen) 
				{
					error("Identifier too long");
					return tkn_error;
				}
				ch = get();
			} while (ch != '\0' && (ISALNUM(ch) || ch == '$' || ch == '_'));
			unget(ch);
			buf[i] = '\0';
			name = buf;
			return dbSymbolTable::add(name, tkn_ident);
		} 
		else 
		{
			char car[50];
			sprintf(car,"Invalid symbol %c", (char)ch);
			error(car);
			return tkn_error;
		}
    }
}

bool Giga_Thread::expect(char const* expected, int token)
{
    int tkn = scan();

    if (tkn != token) 
	{
		if (tkn != tkn_error) 
		{
			char buf[256];
			sprintf(buf, "Token '%s' expected", expected);
			error(buf);
		}
		return false;
    }
    return true;
}

bool Giga_Thread::readCondition()
{
    int i, ch;
    for (i = 0; (ch = get()) != ';' && ch !=  '\0'; i++) 
	{
		if (i+1 == buflen) 
		{
			char* newbuf = new char[buflen*2];
			memcpy(newbuf, buf, buflen*sizeof(char));
			delete[] buf;
			buf = newbuf;
			buflen *= 2;
		}
		buf[i] = ch;
    }

    buf[i] = '\0';

//	if (ch != ';') 
//	{
//		error("unexpected end of input");
//		return false;
//  }
	if(i == 0) return false;

    return true;
}



int Giga_Thread::unupported_types(int type)
{
	IT_IT("Giga_Thread::unupported_types");

	int ret = 1;
	
	switch(type)
	{
		case cli_bool:
		IT_COMMENT("unsupported type: cli_bool");
		error_message += QString("unsupported type: cli_bool") + " ";
		break;
		case cli_int1:
		IT_COMMENT("unsupported type: cli_int1");
		error_message += QString("unsupported type: cli_int1") + " ";
		break;
		case cli_int2:
		IT_COMMENT("unsupported type: cli_int2");
		error_message += QString("unsupported type: cli_int2") + " ";
		break;
		case cli_decimal:
		IT_COMMENT("unsupported type: cli_decimal");
		error_message += QString("unsupported type: cli_decimal") + " ";
		break;
		case cli_pasciiz:  /* pointer to zero terminated string */
		IT_COMMENT("unsupported type: cli_pasciiz");
		error_message += QString("unsupported type: cli_pasciiz") + " ";
		break;
		case cli_cstring:  /* string with counter */
		IT_COMMENT("unsupported type: cli_cstring");
		error_message += QString("unsupported type: cli_cstring") + " ";
		break;
		case cli_array_of_oid:
		IT_COMMENT("unsupported type: cli_array_of_oid");
		error_message += QString("unsupported type: cli_array_of_oid") + " ";
		break;
		case cli_array_of_bool:
		IT_COMMENT("unsupported type: cli_array_of_bool");
		error_message += QString("unsupported type: cli_array_of_bool") + " ";
		break;
		case cli_array_of_int1:
		IT_COMMENT("unsupported type: cli_array_of_int1");
		error_message += QString("unsupported type: cli_array_of_int1") + " ";
		break;
		case cli_array_of_int2:
		IT_COMMENT("unsupported type: cli_array_of_int2");
		error_message += QString("unsupported type: cli_array_of_int2") + " ";
		break;
		case cli_array_of_int4:
		IT_COMMENT("unsupported type: cli_array_of_int4");
		error_message += QString("unsupported type: cli_array_of_int4") + " ";
		break;
		case cli_array_of_int8:
		IT_COMMENT("unsupported type: cli_array_of_int8");
		error_message += QString("unsupported type: cli_array_of_int8") + " ";
		break;
		case cli_array_of_real4:
		IT_COMMENT("unsupported type: cli_array_of_real4");
		error_message += QString("unsupported type: cli_array_of_real4") + " ";
		break;
		case cli_array_of_real8:
		IT_COMMENT("unsupported type: cli_array_of_real8");
		error_message += QString("unsupported type: cli_array_of_real8") + " ";
		break;
		case cli_array_of_decimal:
		IT_COMMENT("unsupported type: cli_array_of_decimal");
		error_message += QString("unsupported type: cli_array_of_decimal") + " ";
		break;
		case cli_array_of_string:/* array of pointers to zero terminated strings */ 
		IT_COMMENT("unsupported type: cli_array_of_string");
		error_message += QString("unsupported type: cli_array_of_string") + " ";
		break;
		case cli_any:     /* use the same type for column as stored in the database */
		IT_COMMENT("unsupported type: cli_any");
		error_message += QString("unsupported type: cli_any") + " ";
		break;
		case cli_datetime: /* time in seconds since 00:00:00 UTC, January 1, 1970. */
		IT_COMMENT("unsupported type: cli_datetime");
		error_message += QString("unsupported type: cli_datetime") + " ";
		break;
		case cli_autoincrement: /* column of int4 type automatically assigned value during record insert */
		IT_COMMENT("unsupported type: cli_autoincrement");
		error_message += QString("unsupported type: cli_autoincrement") + " ";
		break;
		case cli_rectangle: 
		IT_COMMENT("unsupported type: cli_rectangle");
		error_message += QString("unsupported type: cli_rectangle") + " ";
		break;
		case cli_unknown:
		IT_COMMENT("unsupported type: cli_unknown");
		error_message += QString("unsupported type: cli_unknown") + " ";
		break;
		default:
		ret = 0;
		break;
	}

	return ret;
}
