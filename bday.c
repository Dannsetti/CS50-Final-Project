#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define nullptr NULL
#define true 1
#define false 0

typedef unsigned short int Uint16;
typedef unsigned char Uint8;
typedef long int Long;
typedef char bool;

const Long Cbegin = 0;
const Long Cend = -1;
const Long Cfit = -2;
const Long Cerror = -3;
const Uint8 Cedit = 0;
const Uint8 Cdelete = 1;
const char* Cdatabase = "data.dat";
bool quitMe = false;

struct Date;
struct Person;
struct DoubleNode;
typedef struct Date Date;
typedef struct Person Person;
typedef struct DoubleNode DoubleNode;

Date* newDate();
void constructDate(Date* this, Uint8 day, Uint8 month, Uint16 year);
Date setDate(Uint8 day, Uint8 month, Uint16 year);
bool equalsDate(Date d1, Date d2);
Date plusDate(Date d1, Date d2);
Date minusDate(Date d1, Date d2);

Person* newPerson();
Person* constructPerson(Person* this, const char* name, Date birthday);
Person setPerson(const char* name, Date birthday);
bool equalsPerson(Person p1, Person p2);
void printPerson(Person this);
bool savePerson(Person* this, const char* dst, Long pos);
Long savePersonWhereMatches(Person* this, const char* dst);
bool loadPerson(Person* this, const char* src, Long pos);
Long loadPersonWhereMatches(Person* this, Person match, const char* src);

void inputInt(const char* format, void* data);
bool equalsName(char* s1, Uint8 l1, char* s2, Uint8 l2);
Date getCurrentDate();
Uint16 calcAge(Date date);
float calcAgef(Date date);
void skipLines();
void waitInput();

//Date's function
struct Date
{
	Uint8 day, month;
	Uint16 year;
};

Date* newDate()
{
	return malloc(sizeof(Date));
}

void constructDate(Date* this, Uint8 day, Uint8 month, Uint16 year)
{
	this->day = day;
	this->month = month;
	this->year = year;
}

Date setDate(Uint8 day, Uint8 month, Uint16 year)
{
	Date date;
	constructDate(&date,day,month,year);
	return date;
}

bool equalsDate(Date d1, Date d2)
{
    return d1.day == d2.day && d1.month == d2.month && d1.year == d2.year;
}

Date plusDate(Date d1, Date d2)
{
	return setDate(d1.day+d2.day,d1.month+d2.month,d1.year+d2.year);
}

Date minusDate(Date d1, Date d2)
{
	return setDate(d1.day-d2.day,d1.month-d2.month,d1.year-d2.year);
}

bool validateDate(Date date)
{
    Date tmp = getCurrentDate();
    
    if (date.month > 12 || date.month < 1)
    {
        return false;
    }
    
    if (date.year < 0 || date.year > tmp.year)
    {
        return false;
    }
    
    if (date.day < 1 || date.day > 31)
    {
        return false;
    }
    return true;    
}

int compareDate(Date d1, Date d2)
{
    if (d1.month == d2.month && d1.day == d2.day)
    {
        return 0;
    }
    else if (d1.month != d2.month)
    {
        return d1.month > d2.month ? 1 : -1;
    }
    else
    {
        return d1.day > d2.day ? 1 : -1;
    }
}

//Person's function
struct Person
{
	char name[64];
	Uint8 length;
	struct Date birthday;
};

Person* newPerson()
{
	return malloc(sizeof(Person));
}

Person* newConstructPerson(const char* name, Date birthday)
{
    return constructPerson((Person*)newPerson(),name,birthday);
}

Person* constructPerson(Person* this, const char* name, Date birthday)
{
    if (this == nullptr)
    {
        return this;
    }
	bool b = false;
	this->length = 64;
	for (Uint8 i = 0; i != 64; i++)
	{
		this->name[i] = name[i];
		if (name[i] == 0 && b == false)
		{
			this->length = i;
			b = true;
		}
	}
	this->birthday = birthday;
	return this;
}

Person setPerson(const char* name, Date birthday)
{
	Person p;
	constructPerson(&p,name,birthday);
	return p;
}

bool equalsPerson(Person p1, Person p2)
{
    if (!equalsDate(p1.birthday,p2.birthday))
    {
        return false;
    }
    return equalsName(p1.name,p1.length,p2.name,p2.length);
}


void printPerson(Person this)
{
    printf("%s | %0.2d | %0.2d | %0.4d",this.name,this.birthday.day,this.birthday.month,this.birthday.year);
}

bool savePerson(Person* this, const char* dst, Long pos)
{
	FILE* file = fopen(dst,"rb+");
	bool b = false;
	
	if(file == nullptr || this == nullptr)
	{
		return false;
	}
	
	if (pos == Cend)
	{
		fseek(file,0,SEEK_END);
	}
	else if (pos == Cfit)
	{
	    Person tmp;
	    while (!feof(file))
	    {
	        fread(&tmp,sizeof(Person),1,file);
	        if(tmp.length < 1 || tmp.length > 64)
	        {
	            fseek(file,-sizeof(Person),SEEK_CUR);
	            b = true;
	            break;
	        }
	    }
	    if (b == false)
	    {
	        fseek(file,0,SEEK_END);
	    }
	}
	else
	{
	    fseek(file,pos,SEEK_SET);
	}
	
	fwrite(this,sizeof(Person),1,file);
	if (ferror(file))
	{
	    fseek(file,0,SEEK_END);
	    fwrite(this,sizeof(Person),1,file);
	    if(ferror(file))
	    {
	        fclose(file);
	        return false;
	    }
	}
	
	fclose(file);
	return true;
}

bool loadPerson(Person* this, const char* src, Long pos)
{
    FILE* file = fopen(src,"r");
	
	if(file == nullptr || this == nullptr)
	{
		return false;
	}
	
	if(pos >= Cbegin)
	{
		fseek(file,pos,SEEK_SET);
	}
	else
	{
		fseek(file,0,SEEK_END);
	}
	
	fread(this,sizeof(Person),1,file);
	if (ferror(file) || this->length < 1 || this->length > 64)
	{
	    fseek(file,0,SEEK_END);
	    fread(this,sizeof(Person),1,file);
	    if (ferror(file) || this->length < 1 || this->length > 64)
	    {
	        fclose(file);
	        return false;
	    }
	} 
	
	fclose(file);
	return true;
}

Long savePersonWhereMatches(Person* this, const char* src)
{
    Person tmp;
    FILE* file = fopen(src,"rb+");
    
    if(file == nullptr || this == nullptr)
	{
		return Cerror;
	}
	
	while (!feof(file))
	{
	    fread(&tmp,sizeof(Person),1,file);
	    if (equalsName(this->name,this->length,tmp.name,tmp.length))
	    {
	        Long position = ftell(file);
	        fseek(file,position-sizeof(Person),SEEK_SET);
	        fwrite(this,sizeof(Person),1,file);
	        fclose(file);
	        return position;
	    }
	}
	fclose(file);
	return Cerror;
}


Long loadPersonWhereMatches(Person* this, Person match, const char* src)
{
    FILE* file = fopen(src,"r");
    
    if(file == nullptr || this == nullptr)
	{
		return Cerror;
	}
	
	while (!feof(file))
	{
	    fread(this,sizeof(Person),1,file);
	    if (equalsName(this->name,this->length,match.name,match.length))
	    {
	        Long position = ftell(file) - sizeof(Person);
	        fclose(file);
	        return position;
	    }
	} 
	
	fclose(file);
	return Cerror;
}

struct DoubleNode
{
    struct DoubleNode* next, * back;
    void* data;
};

bool extractPersonInNode(Person* this, DoubleNode* node)
{
    if (node == nullptr || this == nullptr)
    {
        return false;
    }
    
    Person* tmp = (Person*)node->data;
    constructPerson(this,tmp->name,tmp->birthday);
    
    return true;
}

void constructNode(DoubleNode* this, DoubleNode* next, DoubleNode* back, void* data)
{
    this->next = next;
    this->back = back;
    this->data = data;
}

bool addNode(DoubleNode** this, void* data, DoubleNode* back)
{
    DoubleNode* ptr;
    ptr = malloc(sizeof(DoubleNode));
    if (ptr == nullptr)
    {
        return false;
    }
    
    if (*this == nullptr)
    {
        *this = ptr;
        constructNode(*this,nullptr,back,data);
    }
    else
    {
        constructNode(ptr,(*this)->next,*this,data);
        (*this)->next = ptr;
        *this = ptr;
    }
    
    return true;
}

bool addSortedPersonToNode(DoubleNode** node, Person* this)
{
    DoubleNode* ptr = *node, * tail = nullptr;
    Person* tmp;
    
    if (this == nullptr)
    {
        return false;
    }
    
    if(*node == nullptr)
    {
        return addNode(node,this,nullptr);
    }
    
    tmp = (Person*) ptr->data;
    if(compareDate(tmp->birthday,this->birthday) <= 0)
    {
        return addNode(node,this,*node);
    }
    else
    {
        tail = ptr->back ? ptr->back : ptr; 
        ptr = ptr->back;
    }
    
    while (ptr != nullptr)
    {
        tmp = (Person*) ptr->data;
        if (compareDate(tmp->birthday,this->birthday) <= 0)
        {
            return addNode(&ptr,this,ptr);
        }
        tail = ptr->back ? ptr->back : ptr;
        ptr = ptr->back;
    }
    tmp = (Person*) tail->data;
    tail->back = malloc(sizeof(DoubleNode));
    if (tail->back == nullptr)
    {
        return false;
    }
    tail->back->data = this;
    tail->back->next = tail;
    tail->back->back = nullptr;
    return true;
}

void clearNode(DoubleNode** this)
{
    DoubleNode* p, *pt = *this;
    int count = 0;
    
    while(pt)
    {
        p = pt;
        pt = pt->back;
        count += sizeof(Person);
        count += sizeof(DoubleNode);
        free((Person*)p->data);
        free(p);
    }
    *this = nullptr;
}

void printPersonNodes(DoubleNode* node)
{
    DoubleNode* ptr = node;
    if (node == nullptr)
    {
        printf("No birthdays in the next 2 months\n");
        return;
    }
    
    while (ptr->back != nullptr)
    {
        ptr = ptr->back;
    }
    
    while(ptr != nullptr)
    {
        Person* tmp = (Person*) ptr->data;
        Uint16 age;
        
        if (tmp->birthday.month > 12)
        {
            tmp->birthday.month -= 12;
            age = calcAge(tmp->birthday) + 1;
        }
        else
        {
            age = calcAge(tmp->birthday);
        }
        printf("%s will be %d in %hhu/%hhu\n\n",tmp->name,age,tmp->birthday.day,tmp->birthday.month);
        ptr = ptr->next;
    }
}

//Header-level function
void inputInt(const char* format, void* data)
{
    char dummy;
    fscanf(stdin,format,data,&dummy);
}

bool equalsName(char* s1, Uint8 l1, char* s2, Uint8 l2)
{
    if (l1 != l2)
    {
        return false;
    }
    for (Uint8 i = 0; i != l1; i++)
    {
        if (s1[i] != s2[i])
        {
            return false;
        }
    }
    
    return true;
}

Date getCurrentDate (void)
{
  time_t curtime;
  struct tm *stime;
  Date date;

  /* Get the current time.  */
  curtime = time (NULL);

  /* Convert it to local time representation.  */
  stime = localtime (&curtime);
  constructDate(&date,stime->tm_mday,stime->tm_mon+1,1900+stime->tm_year);
  return date;
}

Uint16 calcAge(Date date)
{
	Date today = getCurrentDate();
	today.year = today.year - date.year;
	float month = today.month == date.month ? 0.5 : 0;
	float day = today.day == date.day ? 0.5 : 0;
	return (Uint16)( (float)(today.year) + (float)(month+day));
}

float calcAgef(Date date)
{
	Date today = minusDate(getCurrentDate(),date);
	return (float)today.year + (float)(today.month/12) + (float)(today.day/365);
}

void listNextBD()
{
    Person tmp;
    Date today = getCurrentDate();
    Uint8 dif;
    DoubleNode* list = nullptr;
    FILE* file = fopen(Cdatabase,"rb+");
    if (file == nullptr)
    {
        printf("Failed to open the database.\nPlease close and reopen the software!\n");
        return;
    }
    Long i = 0;
    while(loadPerson(&tmp,Cdatabase,i*sizeof(Person)))
    {
        i++;
        if(ferror(file))
        {
            printf("Failed to open the database.\nPlease close and reopen the software!\n");
            clearNode(&list);
            return;
        }
        
        if (tmp.birthday.month < today.month)
        {
            tmp.birthday.month += 12;
        }
        dif = tmp.birthday.month - today.month;
        dif = tmp.birthday.day < today.day && dif == 0 ? 99 : dif;
        
        if ((dif == 0 || dif == 1) && tmp.length >= 1 && tmp.length < 64)
        {
            if (addSortedPersonToNode(&list,newConstructPerson(tmp.name,tmp.birthday)) == false)
            {
                printf("Failed to request memory.\nPlease close and reopen the software!\n");
                clearNode(&list);
                return;
            }
        }
    }
    printf("\n");
    if (list == nullptr)
    {
        printf("Failed to request memory.\nPlease close and reopen the software!\n");
        return;
    }
    
    printPersonNodes(list);
    clearNode(&list);
    fclose(file);
}

void registerEvent()
{
    Person tmp;
    int choose;
    fflush(stdout);
    fflush(stdin);
    L_BEG:
    printf("Type Person's name:");
    fgets(tmp.name, 64, stdin);
    L_DATE:
    printf("Type Person's day of birthday:");
    inputInt("%hd%c",(short int*) &(tmp.birthday.day));
    printf("Type Person's month:");
    inputInt("%hd%c",(short int*) &(tmp.birthday.month));
    printf("Type Person's year:");
    inputInt("%hd%c", &(tmp.birthday.year));
    constructPerson(&tmp,tmp.name,tmp.birthday);
    if(tmp.length != 0)
    {
        if (tmp.name[tmp.length-1] == '\n')
        {
            tmp.name[tmp.length-1] = '\0';
            tmp.length -= 1;
        }
    }
    if (validateDate(tmp.birthday) == false)
    {
        skipLines();
        printf("Input for DATE has the following format:");
        printf("\n\tDay: 1 - 30/31\n\tMonth: 1 - 12\n\tYear: 0 - Today\n");
        printf("Type Person's name: %s\n",tmp.name);
        goto L_DATE;
    }    
       
    printf("You typed the following: ");
    printPerson(tmp);
    L_INPUT:
    printf("\n1 - Confirm\n");
    printf("2 - Edit\n");
    printf("3 - Cancel\n");
    inputInt("%d%c", &choose);
    fflush(stdout);
    fflush(stdin);
    switch (choose)
    {
        case 1:
            if (savePersonWhereMatches(&tmp,Cdatabase) <= Cerror)
            {
                if (savePerson(&tmp,Cdatabase,Cfit))
                {
                    printf("Register made succefully!\n");
                }
                else
                {
                    printf("Failed to register new event!\nClose and reopen the software!\n");
                }
            }
            break;
        case 2:
            skipLines();
            goto L_BEG;
        case 3:
            break;
        default:
            printf("Unkown command!\n");
            waitInput();
            skipLines();
            goto L_INPUT;
            break;
    }
}

void editEvent(Person this, Long pos)
{
    Person tmp;
    int choose;
    fflush(stdout);
    fflush(stdin);
    L_BEG2:
    printf("Found: ");
    printPerson(this);
    printf("\nEdit person:\n");
    printf("Type Person's name:");
    fgets(tmp.name, 64, stdin);
    L_DATE2:
    printf("Type Person's day of birthday:");
    inputInt("%hd%c",(short int*) &(tmp.birthday.day));
    printf("Type Person's month:");
    inputInt("%hd%c",(short int*) &(tmp.birthday.month));
    printf("Type Person's year:");
    inputInt("%hd%c", &(tmp.birthday.year));
    fflush(stdout);
    constructPerson(&tmp,tmp.name,tmp.birthday);
    if(tmp.length != 0)
    {
        if (tmp.name[tmp.length-1] == '\n')
        {
            tmp.name[tmp.length-1] = '\0';
            tmp.length -= 1;
        }
    }
    if (validateDate(tmp.birthday) == false)
    {
        skipLines();
        printf("Input for DATE has the following format:");
        printf("\n\tDay: 1 - 30/31\n\tMonth: 1 - 12\n\tYear: 0 - Today\n");
        printf("\nFound: ");
        printPerson(this);
        printf("Edit person:\n");
        printf("Type Person's name: %s\n",tmp.name);
        goto L_DATE2;
    }    
       
    printf("You typed the following: ");
    printPerson(tmp);
    L_INPUT2:
    printf("\n1 - Confirm\n");
    printf("2 - Edit\n");
    printf("3 - Cancel\n");
    inputInt("%d%c", &choose);
    fflush(stdout);
    fflush(stdin);
    switch (choose)
    {
        case 1:
            if (savePerson(&tmp,Cdatabase,pos))
            {
                printf("Register made succefully!\n");
            }
            else
            {
                printf("Failed to register new event!\nClose and reopen the software!\n");
            }
            break;
        case 2:
            skipLines();
            goto L_BEG2;
        case 3:
            break;
        default:
            printf("Unkown command!\n");
            waitInput();
            skipLines();
            goto L_INPUT2;
            break;
    }
}

void deleteEvent(Person this, Long pos)
{
    Person tmp;
    int choose;
    fflush(stdout);
    fflush(stdin);
    L_delMenu:
    printf("Found: ");
    printPerson(this);
    printf("\nAre you sure you want to delete this person?\n");
    printf("1 - Yes\n2 - No\n");
    inputInt("%d%c",&choose);
    switch (choose)
    {
        case 1:
            tmp.length = 0;
            tmp.name[0] = 0;
            if (savePerson(&tmp,Cdatabase,pos) <= Cerror)
            {
                printf("Failed to delete person\n");     
            }
            else
            {
                printf("Person Deleted!\n");
            }
            break;
        case 2:
            break;
        default:
            printf("Unkown command!\n");
            waitInput();
            skipLines();
            goto L_delMenu;
    }
}


void findEvent(Uint8 flag)
{
    int command;
    Long id;
    Person tmp;
    
    L_findMenu:
    skipLines();
    printf("1 - Search by id\n");
    printf("2 - Search by Name\n");
    printf("3 - Return\n");
    inputInt("%d%c",&command);
    switch (command)
    {
        case 1:
            printf("Type ID: ");
            inputInt("%ld%c",&id);
            id -= 1;
            if (id < 0)
            {
                printf("ID not found\n");
                waitInput();
                goto L_findMenu;
            }
            if (loadPerson(&tmp,Cdatabase,id*sizeof(Person)) == false)
            {
                printf("ID not found\n");
                waitInput();
                goto L_findMenu; 
            }
            if (flag == Cedit)
                editEvent(tmp,id*sizeof(Person));
            else if (flag == Cdelete)
                deleteEvent(tmp,id*sizeof(Person));
            break;
        case 2:
            printf("Type person's name: ");
            fgets(tmp.name,64,stdin);
            constructPerson(&tmp,tmp.name,tmp.birthday);
            if(tmp.length != 0)
            {
                if (tmp.name[tmp.length-1] == '\n')
                {
                    tmp.name[tmp.length-1] = '\0';
                    tmp.length -= 1;
                }
            }
            id = loadPersonWhereMatches(&tmp,tmp,Cdatabase);
            if (id <= Cerror)
            {
                printf("Name not found\n");
                waitInput();
                goto L_findMenu; 
            }
            if(flag == Cedit)
                editEvent(tmp,id);
            else if (flag == Cdelete)
                deleteEvent(tmp,id);
            break;
        case 3:
            break;
        default:
            printf("Unkown command!\n");
            waitInput();
            goto L_findMenu;
    }
}

bool listEvents(Long p0, Long count)
{
    FILE* file = fopen(Cdatabase,"r");
    Person tmp;
    Date today;
    if (file == nullptr)
    {
        return false;
    }
    today = getCurrentDate();
    fseek(file,p0,SEEK_SET);
    for (Long i = p0; i < p0 + count; i++)
    {
        fread(&tmp,sizeof(Person),1,file);
        if (ferror(file) || feof(file))
        {
            fclose(file);
            return false;
        }
        else if ( !(tmp.length < 1 || tmp.length > 64) )
        {
            Long id = (i % sizeof(Person)) + (i/sizeof(Person));
            printf("%ld | ", id + 1);
            printPerson(tmp);
            //Uint16 age = calcAge(tmp.birthday);
            //age += (today.month > tmp.birthday.month) ? 1 : 0;
            printf("\n");
            fflush(stdout);
        }
    }
    
    fclose(file);
    return true;
}

void listAllEvents()
{
    Long pos = 0;
    const Long count = 13;
    Long page = 0;
    Uint8 command;
    bool excp;
    while (1)
    {
        //Recalc position
        pos = page*count*sizeof(Person);
        skipLines();
        //print submenu and guidelines
        printf("1 - Previous Page\n");
        printf("2 - Next Page\n");
        printf("3 - Return\n");
        printf("Page: %ld\n", page+1);
        printf("ID\t|    Name    |    Day    |    Month    |    Year\n\n\r");
        fflush(stdout);
        excp = listEvents(pos,count);
        //Get command
        inputInt("%d",(int*)&command);
        //Parse command
        if (command == 1)
        {
            page = page == 0 ? 0 : page-1;
        }
        else if (command == 2)
        {
            page = excp == false ? page : page+1;
        }
        else if(command == 3)
        {
            return;
        }
    }
}

void waitInput()
{
    char c;
    printf("Press \"Enter\" to continue");
    fflush(stdin);
    fscanf(stdin,"%c",&c);
}

void skipLines()
{
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\r");
    fflush(stdout);
}

void printMainMenu()
{
    printf("1 - Up Coming Birthdays\n");
    printf("2 - Register New Event\n");
    printf("3 - Edit Event\n");
    printf("4 - Delete Event\n");
    printf("5 - List All Events\n");
    printf("6 - Exit\n");
    fflush(stdout);
}

void parseCommand(Uint8 command)
{
    switch (command)
    {
        case 1:
            listNextBD();
            break;
        case 2:
            registerEvent();
            break;
        case 3:
            findEvent(Cedit);
            break;
        case 4:
            findEvent(Cdelete);
            break;
        case 5:
            listAllEvents();
            break;
        case 6:
            quitMe = true; 
            break;
        default:
            printf("Unkown command");
            break;
    }
}


int main()
{   
    Uint8 command;
    FILE* file = fopen(Cdatabase,"rb+");
    skipLines();
    printf("MY B'DAY ORGANIZER\n\n\n\n\n\n\n\n\n");
    if (file == nullptr)
    {
        file = fopen(Cdatabase,"w");
        if (file == nullptr)
        {
            printf("Failed to create Database!\nClose and reopen the software!");
            waitInput();
            return 1;
        }
    }
    fclose(file);
    while (1)
    {
        printMainMenu();
        inputInt("%d%c",(int*)&command);
        parseCommand(command);
        if (quitMe == false)
        {
            waitInput();
            skipLines();
        }
        else
        {
            skipLines();
            printf("Good Bye!\n");
            waitInput();
            break;
        }
    }
       
    return 0;   
}
