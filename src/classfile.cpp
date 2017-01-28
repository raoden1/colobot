// ClassFile.cpp
//
// d�finition des m�thodes pour la classe FILE



// Variables statiques

static CBotClass*	m_pClassFILE;
static CBotProgram*	m_pFuncFile;
static int			m_CompteurFileOpen = 0;
static char*		m_filesDir;



// Pr�pare un nom de fichier.

void PrepareFilename(CBotString &filename)
{
	int			pos;

	pos = filename.ReverseFind('\\');
	if ( pos > 0 )
	{
		filename = filename.Mid(pos+1);  // enl�ve les dossiers
	}

	pos = filename.ReverseFind('/');
	if ( pos > 0 )
	{
		filename = filename.Mid(pos+1);  // aussi ceux avec /
	}

	pos = filename.ReverseFind(':');
	if ( pos > 0 )
	{
		filename = filename.Mid(pos+1);  // enl�ve aussi la lettre d'unit� C:
	}

	filename = CBotString(m_filesDir) + CBotString("\\") + filename;
}


// constructeur de la classe
// re�ois le nom du fichier en param�tre

// ex�cution
BOOL rfconstruct (CBotVar* pThis, CBotVar* pVar, CBotVar* pResult, int& Exception) 
{
	CBotString	mode;

	// accepte sans param�tre
	if ( pVar == NULL ) return TRUE;

	// qui doit �tre une cha�ne de caract�res
	if ( pVar->GivType() != CBotTypString ) { Exception = CBotErrBadString; return FALSE; }

	CBotString	filename = pVar->GivValString();
	PrepareFilename(filename);

	// il peut y avoir  un second param�tre
	pVar = pVar->GivNext();
	if ( pVar != NULL )
	{
		// r�cup�re le mode
		mode = pVar->GivValString();
		if ( mode != "r" && mode != "w" ) { Exception = CBotErrBadParam; return FALSE; }

		// pas de 3e param�tre
		if ( pVar->GivNext() != NULL ) { Exception = CBotErrOverParam; return FALSE; } 
	}

	// enregistre le nom du fichier
	pVar = pThis->GivItem("filename");
	pVar->SetValString(filename);

	if ( ! mode.IsEmpty() )
	{
		// ouvre le ficher demand�
		FILE*	pFile = fopen( filename, mode );
		if ( pFile == NULL ) { Exception = CBotErrFileOpen; return FALSE; }

		m_CompteurFileOpen ++;

		// enregiste le canal du fichier
		pVar = pThis->GivItem("handle");
		pVar->SetValInt((long)pFile);
	}

	return TRUE;
}

// compilation
CBotTypResult cfconstruct (CBotVar* pThis, CBotVar* &pVar)
{
	// accepte sans param�tre
	if ( pVar == NULL ) return CBotTypResult( 0 );

	// qui doit �tre une chaine
	if ( pVar->GivType() != CBotTypString )
						return CBotTypResult( CBotErrBadString );

	// il peut y avoir un second param�tre
	pVar = pVar->GivNext();
	if ( pVar != NULL )
	{
		// qui doit �tre une chaine
		if ( pVar->GivType() != CBotTypString )
							return CBotTypResult( CBotErrBadString );
		// pas de 3e param�tre
		if ( pVar->GivNext() != NULL ) return CBotTypResult( CBotErrOverParam );
	}

	// le r�sultat est de type void (constructeur)
	return CBotTypResult( 0 );
}


// destructeur de la classe

// ex�cution
BOOL rfdestruct (CBotVar* pThis, CBotVar* pVar, CBotVar* pResult, int& Exception)
{
	// r�cup�re l'�l�ment "handle"
	pVar = pThis->GivItem("handle");

	// pas ouvert ? pas de probl�me
	if ( pVar->GivInit() != IS_DEF) return TRUE;

	FILE* pFile= (FILE*)pVar->GivValInt();
	fclose(pFile);
	m_CompteurFileOpen --;

	pVar->SetInit(IS_NAN);

	return TRUE;
}


// m�thode FILE :: open
// re�ois le mode r/w en param�tre

// ex�cution
BOOL rfopen (CBotVar* pThis, CBotVar* pVar, CBotVar* pResult, int& Exception)
{
	// il doit y avoir un param�tre
	if ( pVar == NULL ) { Exception = CBotErrLowParam; return FALSE; }

	// qui doit �tre une cha�ne de caract�res
	if ( pVar->GivType() != CBotTypString ) { Exception = CBotErrBadString; return FALSE; }

	// il peut y avoir  un second param�tre
	if ( pVar->GivNext() != NULL )
	{
		// dans ce cas le premier param�tre est le nom du fichier
		CBotString	filename = pVar->GivValString();
		PrepareFilename(filename);

		// enregistre le nom du fichier
		CBotVar* pVar2 = pThis->GivItem("filename");
		pVar2->SetValString(filename);

		// param�tre suivant est le mode
		pVar = pVar -> GivNext();
	}

	CBotString	mode = pVar->GivValString();
	if ( mode != "r" && mode != "w" ) { Exception = CBotErrBadParam; return FALSE; }

	// pas de 3e param�tre
	if ( pVar->GivNext() != NULL ) { Exception = CBotErrOverParam; return FALSE; }

	// r�cup�re l'�l�ment "handle"
	pVar = pThis->GivItem("handle");

	// qui doit pas �tre initialis�
	if ( pVar->GivInit() == IS_DEF) { Exception = CBotErrFileOpen; return FALSE; }

	// reprend le nom du fichier
	pVar = pThis->GivItem("filename");
	CBotString	filename = pVar->GivValString();

	PrepareFilename(filename);  // si le nom a �t� attribu� par h.filename = "...";

	// ouvre le ficher demand�
	FILE*	pFile = fopen( filename, mode );
	if ( pFile == NULL )
	{
		pResult->SetValInt(FALSE);
		return TRUE;
	}

	m_CompteurFileOpen ++;

	// enregiste le canal du fichier
	pVar = pThis->GivItem("handle");
	pVar->SetValInt((long)pFile);

	pResult->SetValInt(TRUE);
	return TRUE;
}

// compilation
CBotTypResult cfopen (CBotVar* pThis, CBotVar* &pVar)
{
	// il doit y avoir un param�tre
	if ( pVar == NULL ) return CBotTypResult( CBotErrLowParam );

	// qui doit �tre une chaine
	if ( pVar->GivType() != CBotTypString )
						return CBotTypResult( CBotErrBadString );

	// il peut y avoir  un second param�tre
	pVar = pVar->GivNext();
	if ( pVar != NULL )
	{
		// qui doit �tre une chaine
		if ( pVar->GivType() != CBotTypString )
						return CBotTypResult( CBotErrBadString );

		// pas de 3e param�tre
		if ( pVar->GivNext() != NULL ) return CBotTypResult( CBotErrOverParam );
	}

	// le r�sultat est de type bool
	return CBotTypResult(CBotTypBoolean);
}

	
// m�thode FILE :: close

// ex�cution
BOOL rfclose (CBotVar* pThis, CBotVar* pVar, CBotVar* pResult, int& Exception)
{
	// il ne doit pas y avoir de param�tre
	if ( pVar != NULL ) return CBotErrOverParam;

	// r�cup�re l'�l�ment "handle"
	pVar = pThis->GivItem("handle");

	if ( pVar->GivInit() != IS_DEF) { Exception = CBotErrNotOpen; return FALSE; }

	FILE* pFile= (FILE*)pVar->GivValInt();
	fclose(pFile);
	m_CompteurFileOpen --;

	pVar->SetInit(IS_NAN);

	return TRUE;
}

// compilation
CBotTypResult cfclose (CBotVar* pThis, CBotVar* &pVar)
{
	// il ne doit pas y avoir de param�tre
	if ( pVar != NULL ) return CBotTypResult( CBotErrOverParam );

	// la fonction retourne un r�sultat "void"
	return CBotTypResult( 0 );
}

// m�thode FILE :: writeln

// ex�cution
BOOL rfwrite (CBotVar* pThis, CBotVar* pVar, CBotVar* pResult, int& Exception)
{
	// il doit y avoir un param�tre
	if ( pVar == NULL ) { Exception = CBotErrLowParam; return FALSE; }

	// qui doit �tre une cha�ne de caract�res
	if ( pVar->GivType() != CBotTypString ) { Exception = CBotErrBadString; return FALSE; }

	CBotString param = pVar->GivValString();

	// r�cup�re l'�l�ment "handle"
	pVar = pThis->GivItem("handle");

	if ( pVar->GivInit() != IS_DEF) { Exception = CBotErrNotOpen; return FALSE; }

	FILE* pFile= (FILE*)pVar->GivValInt();
	
	int res = fputs(param.operator+("\n"), pFile);

	// en cas d'erreur g�n�re une exception
	if ( res < 0 ) { Exception = CBotErrWrite; return FALSE; }

	return TRUE;
}

// compilation
CBotTypResult cfwrite (CBotVar* pThis, CBotVar* &pVar)
{
	// il doit y avoir un param�tre
	if ( pVar == NULL ) return CBotTypResult( CBotErrLowParam );

	// qui doit �tre une cha�ne de caract�res
	if ( pVar->GivType() != CBotTypString ) return CBotTypResult( CBotErrBadString );

	// pas d'autre param�tre
	if ( pVar->GivNext() != NULL ) return CBotTypResult( CBotErrOverParam );

	// la fonction retourne un r�sultat void
	return CBotTypResult( 0 );
}

// m�thode FILE :: readln

// ex�cution
BOOL rfread (CBotVar* pThis, CBotVar* pVar, CBotVar* pResult, int& Exception)
{
	// il ne doit pas y avoir de param�tre
	if ( pVar != NULL ) { Exception = CBotErrOverParam; return FALSE; }

	// r�cup�re l'�l�ment "handle"
	pVar = pThis->GivItem("handle");

	if ( pVar->GivInit() != IS_DEF) { Exception = CBotErrNotOpen; return FALSE; }

	FILE* pFile= (FILE*)pVar->GivValInt();

	char	chaine[2000];
	int		i;
	for ( i = 0 ; i < 2000 ; i++ ) chaine[i] = 0;

	fgets(chaine, 1999, pFile);

	for ( i = 0 ; i < 2000 ; i++ ) if (chaine[i] == '\n') chaine[i] = 0;

	// en cas d'erreur g�n�re une exception
	if ( ferror(pFile) ) { Exception = CBotErrRead; return FALSE; }

	pResult->SetValString( chaine );

	return TRUE;
}

// compilation
CBotTypResult cfread (CBotVar* pThis, CBotVar* &pVar)
{
	// il ne doit pas y avoir de param�tre
	if ( pVar != NULL ) return CBotTypResult( CBotErrOverParam );

	// la fonction retourne un r�sultat "string"
	return CBotTypResult( CBotTypString );
}
// m�thode FILE :: readln


// ex�cution
BOOL rfeof (CBotVar* pThis, CBotVar* pVar, CBotVar* pResult, int& Exception)
{
	// il ne doit pas y avoir de param�tre
	if ( pVar != NULL ) { Exception = CBotErrOverParam; return FALSE; }

	// r�cup�re l'�l�ment "handle"
	pVar = pThis->GivItem("handle");

	if ( pVar->GivInit() != IS_DEF) { Exception = CBotErrNotOpen; return FALSE; }

	FILE* pFile= (FILE*)pVar->GivValInt();

	pResult->SetValInt( feof( pFile ) );

	return TRUE;
}

// compilation
CBotTypResult cfeof (CBotVar* pThis, CBotVar* &pVar)
{
	// il ne doit pas y avoir de param�tre
	if ( pVar != NULL ) return CBotTypResult( CBotErrOverParam );

	// la fonction retourne un r�sultat booleen
	return CBotTypResult( CBotTypBoolean );
}





void InitClassFILE()
{
// cr�e une classe pour la gestion des fichiers
// l'utilisation en est la suivante:
// file canal( "NomFichier.txt" )
// canal.open( "r" );	// ouvre en lecture
// s = canal.readln( ); // lit une ligne
// canal.close();		// referme le fichier

	// cr�e la classe FILE
	m_pClassFILE	= new CBotClass("file", NULL);
	// ajoute le composant ".filename"
	m_pClassFILE->AddItem("filename", CBotTypString);
	// ajoute le composant ".handle"
	m_pClassFILE->AddItem("handle", CBotTypInt, PR_PRIVATE);

	// d�fini un constructeur et un destructeur
	m_pClassFILE->AddFunction("file", rfconstruct, cfconstruct );
	m_pClassFILE->AddFunction("~file", rfdestruct, NULL );

	// d�fini les m�thodes associ�es
	m_pClassFILE->AddFunction("open", rfopen, cfopen );
	m_pClassFILE->AddFunction("close", rfclose, cfclose );
	m_pClassFILE->AddFunction("writeln", rfwrite, cfwrite );
	m_pClassFILE->AddFunction("readln", rfread, cfread );
	m_pClassFILE->AddFunction("eof", rfeof, cfeof );

	m_pFuncFile = new CBotProgram( );
	CBotStringArray ListFonctions;
	m_pFuncFile->Compile( "public file openfile(string name, string mode) {return new file(name, mode);}", ListFonctions);
	m_pFuncFile->SetIdent(-2);	// identificateur sp�cial pour RestoreState dans cette fonction
}
