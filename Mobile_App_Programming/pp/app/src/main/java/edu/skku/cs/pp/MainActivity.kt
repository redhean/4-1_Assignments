package edu.skku.cs.pp

import android.app.Activity
import android.app.AlertDialog
import android.content.Context
import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.*
import com.google.gson.Gson
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import okhttp3.*
import okhttp3.MediaType.Companion.toMediaTypeOrNull
import okhttp3.RequestBody.Companion.toRequestBody
import org.json.JSONObject
import java.io.IOException
import java.time.LocalDate


class MainActivity : AppCompatActivity() {

    companion object{
        const val EXT_NAME = "calendar_name"
        const val EXT_DATE = "date"
        const val EXT_CHK = "yes"
    }

    data class Calendar(
        val name: String,
        val events: List<Event>
    )

    data class Event(
        val date: String,
        val title: String,
        val description: String,
        val check: Boolean
    )

    class CustomAdapter(private val context: Context, private val items: MutableList<Event>) : BaseAdapter() {
        override fun getCount(): Int {
            return items.size
        }

        override fun getItem(p0: Int): Any {
            return 0
        }

        override fun getItemId(p0: Int): Long {
            return 0
        }

        override fun getView(p0: Int, p1: View?, p2: ViewGroup?): View {
            val inflater: LayoutInflater = LayoutInflater.from(context)
            val view : View = inflater.inflate(R.layout.todolist_item,null)

            val todolistTitle = view.findViewById<TextView>(R.id.titleItem2)
            val todolistContent = view.findViewById<CheckBox>(R.id.checkBox)


            todolistTitle.text = items[p0].title
            todolistContent.text = items[p0].description
            todolistContent.isChecked = items[p0].check

            // edit button and delete button
            val editBtn = view.findViewById<ImageView>(R.id.fix2)
            val deleteBtn = view.findViewById<ImageView>(R.id.delete2)
            val activity = context as Activity
            val calendarTitle = activity.findViewById<TextView>(R.id.title)
            val currentDate = activity.findViewById<TextView>(R.id.currentDate)
            val client = OkHttpClient()

            todolistContent.setOnCheckedChangeListener { compoundButton, isChecked ->
                val item = items[p0]
                var jsonEventData : String

                if(isChecked){
                    jsonEventData = """
                    {
                        "calendar": "${calendarTitle.text.toString()}",
                        "date": "${item.date}",
                        "title": "${item.title}",
                        "description": "${item.description}",
                        "beforeTitle": "${item.title}",
                        "beforeDesc": "${item.description}",
                        "check": true
                    }
                    """.trimIndent()
                }
                else{
                    jsonEventData = """
                    {
                        "calendar": "${calendarTitle.text.toString()}",
                        "date": "${item.date}",
                        "title": "${item.title}",
                        "description": "${item.description}",
                        "beforeTitle": "${item.title}",
                        "beforeDesc": "${item.description}",
                        "check": false
                    }
                    """.trimIndent()
                }

                val url = "url_link"
                val requestBody = jsonEventData.toRequestBody("application/json".toMediaTypeOrNull())
                val request = Request.Builder().url(url).put(requestBody).build()

                client.newCall(request).enqueue(object : okhttp3.Callback {
                    override fun onFailure(call: okhttp3.Call, e: IOException) {
                        e.printStackTrace()
                    }

                    override fun onResponse(call: okhttp3.Call, response: okhttp3.Response) {
                        if (!response.isSuccessful) throw IOException("Unexpected code $response")
                    }
                })
            }

            editBtn.setOnClickListener {
                val intent = Intent(context, AddTodolist::class.java).apply {
                    putExtra(EXT_NAME, calendarTitle.text.toString())
                    putExtra(EXT_DATE, currentDate.text.toString())
                    putExtra(EXT_CHK, "no")
                    putExtra("beforeTitle", items[p0].title)
                    putExtra("beforeDesc", items[p0].description)
                }
                context.startActivity(intent)
            }

            deleteBtn.setOnClickListener {
                // Delete button 클릭 시 삭제 요청 처리
                val alertDialogBuilder = AlertDialog.Builder(context)
                alertDialogBuilder.setTitle("Delete Confirmation")
                alertDialogBuilder.setMessage("Are you sure you want to delete?")

                alertDialogBuilder.setPositiveButton("Confirm"){ dialog, which ->
                    val item = items[p0]

                    val jsonEventData = """
                    {
                        "calendar": "${calendarTitle.text.toString()}",
                        "date": "${item.date}",
                        "title": "${item.title}",
                        "description": "${item.description}",
                        "check": ${item.check}
                    }
                    """.trimIndent()

                    val url = "url_link"
                    val requestBody = jsonEventData.toRequestBody("application/json".toMediaTypeOrNull())
                    val request = Request.Builder().url(url).delete(requestBody).build()

                    client.newCall(request).enqueue(object : okhttp3.Callback {
                        override fun onFailure(call: okhttp3.Call, e: IOException) {
                            e.printStackTrace()
                        }

                        override fun onResponse(call: okhttp3.Call, response: okhttp3.Response) {
                            if (!response.isSuccessful) throw IOException("Unexpected code $response")
                            else {
                                (context as Activity).runOnUiThread {
                                    items.remove(item)
                                    notifyDataSetChanged()
                                }
                            }
                        }
                    })
                }
                alertDialogBuilder.setNegativeButton("Cancel"){ dialog, which ->

                }
                val alertDialog = alertDialogBuilder.create()
                alertDialog.show()
            }
            return view
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        supportActionBar?.setTitle("DayFlow")

        // Set title
        val calendarTitle = intent.getStringExtra(StartActivity.EXT_NAME)

        val title = findViewById<TextView>(R.id.title)
        title.text = calendarTitle

        // Get request
        val calendarList = mutableListOf<Calendar>()
        val client = OkHttpClient()
        val gson = Gson()
        val url = "url_link"

        val request = Request.Builder().url(url).build()

        client.newCall(request).enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                e.printStackTrace()
            }

            override fun onResponse(call: Call, response: Response) {
                if(!response.isSuccessful) throw IOException("Unexpected code $response")

                val responseBody = response.body!!.string()

                if (responseBody != null) {
                    val jsonObject = JSONObject(responseBody)
                    val calendarListJsonArray = jsonObject.getJSONArray("calendars")

                    for (i in 0 until calendarListJsonArray.length()) {
                        val calendarJsonObject = calendarListJsonArray.getJSONObject(i)
                        val calendarName = calendarJsonObject.getString("name")
                        val eventsJsonArray = calendarJsonObject.getJSONArray("events")

                        val eventList = mutableListOf<Event>()

                        for (j in 0 until eventsJsonArray.length()) {
                            val eventJsonObject = eventsJsonArray.getJSONObject(j)
                            val eventDate = eventJsonObject.getString("date")
                            val eventTitle = eventJsonObject.getString("title")
                            val eventDescription = eventJsonObject.getString("description")
                            val eventCheck = eventJsonObject.getBoolean("check")

                            val event = Event(
                                eventDate,
                                eventTitle,
                                eventDescription,
                                eventCheck
                            )
                            eventList.add(event)
                        }

                        val calendar = Calendar(calendarName, eventList)
                        calendarList.add(calendar)
                    }
                }

                CoroutineScope(Dispatchers.Main).launch{
                    val currentDate = findViewById<TextView>(R.id.currentDate)
                    val tempDate = LocalDate.now()
                    currentDate.text = tempDate.toString()

                    val initEvents = mutableListOf<Event>()

                    for (calendar in calendarList) {
                        if (calendar.name == title.text) {
                            val events = calendar.events.filter { it.date == tempDate.toString() }
                            initEvents.addAll(events)
                        }
                    }
                    val initAdapter = MainActivity.CustomAdapter(this@MainActivity, initEvents)
                    val initList = findViewById<ListView>(R.id.todolist)
                    initList.adapter = initAdapter
                }
            }
        })

        // Change event
        val mainCalendar = findViewById<CalendarView>(R.id.calendar)
        val currentDate = findViewById<TextView>(R.id.currentDate)
        val tempDate = LocalDate.now()
        currentDate.text = tempDate.toString()
        var selectedDate = tempDate.toString()

        mainCalendar.setOnDateChangeListener { calendarView, year, month, dayOfMonth ->
            selectedDate = String.format("%04d-%02d-%02d", year, month + 1, dayOfMonth)
            currentDate.text = selectedDate

            val filteredEvents = mutableListOf<Event>()
            val calendarList = mutableListOf<Calendar>()
            val client = OkHttpClient()
            val gson = Gson()
            val url = "url_link"

            val request = Request.Builder().url(url).build()

            client.newCall(request).enqueue(object : Callback {
                override fun onFailure(call: Call, e: IOException) {
                    e.printStackTrace()
                }

                override fun onResponse(call: Call, response: Response) {
                    if(!response.isSuccessful) throw IOException("Unexpected code $response")

                    val responseBody = response.body!!.string()

                    if (responseBody != null) {
                        val jsonObject = JSONObject(responseBody)
                        val calendarListJsonArray = jsonObject.getJSONArray("calendars")

                        for (i in 0 until calendarListJsonArray.length()) {
                            val calendarJsonObject = calendarListJsonArray.getJSONObject(i)
                            val calendarName = calendarJsonObject.getString("name")
                            val eventsJsonArray = calendarJsonObject.getJSONArray("events")

                            val eventList = mutableListOf<Event>()

                            for (j in 0 until eventsJsonArray.length()) {
                                val eventJsonObject = eventsJsonArray.getJSONObject(j)
                                val eventDate = eventJsonObject.getString("date")
                                val eventTitle = eventJsonObject.getString("title")
                                val eventDescription = eventJsonObject.getString("description")
                                val eventCheck = eventJsonObject.getBoolean("check")

                                val event = Event(
                                    eventDate,
                                    eventTitle,
                                    eventDescription,
                                    eventCheck
                                )
                                eventList.add(event)
                            }

                            val calendar = Calendar(calendarName, eventList)
                            calendarList.add(calendar)
                        }
                    }

                    CoroutineScope(Dispatchers.Main).launch{
                        for (calendar in calendarList) {
                            if (calendar.name == title.text) {
                                val events = calendar.events.filter { it.date == selectedDate }
                                filteredEvents.addAll(events)
                            }
                        }
                        val todoAdapter = MainActivity.CustomAdapter(this@MainActivity, filteredEvents)
                        val todoList = findViewById<ListView>(R.id.todolist)
                        todoList.adapter = todoAdapter
                    }
                }
            })
        }

        // Add todolist
        val addTodoBtn = findViewById<ImageView>(R.id.addtodo)

        addTodoBtn.setOnClickListener {
            val intent = Intent(this, AddTodolist::class.java).apply {
                putExtra(EXT_NAME, calendarTitle)
                putExtra(EXT_DATE, selectedDate)
                putExtra(EXT_CHK, "yes")
            }
            startActivity(intent)
        }
    }

    override fun onResume() {
        super.onResume()

        // Set title
        val calendarTitle = intent.getStringExtra(StartActivity.EXT_NAME)

        val title = findViewById<TextView>(R.id.title)
        title.text = calendarTitle

        // Get request
        val calendarList = mutableListOf<Calendar>()
        val client = OkHttpClient()
        val gson = Gson()
        val url = "url_link"

        val request = Request.Builder().url(url).build()

        client.newCall(request).enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                e.printStackTrace()
            }

            override fun onResponse(call: Call, response: Response) {
                if(!response.isSuccessful) throw IOException("Unexpected code $response")

                val responseBody = response.body!!.string()

                if (responseBody != null) {
                    val jsonObject = JSONObject(responseBody)
                    val calendarListJsonArray = jsonObject.getJSONArray("calendars")

                    for (i in 0 until calendarListJsonArray.length()) {
                        val calendarJsonObject = calendarListJsonArray.getJSONObject(i)
                        val calendarName = calendarJsonObject.getString("name")
                        val eventsJsonArray = calendarJsonObject.getJSONArray("events")

                        val eventList = mutableListOf<Event>()

                        for (j in 0 until eventsJsonArray.length()) {
                            val eventJsonObject = eventsJsonArray.getJSONObject(j)
                            val eventDate = eventJsonObject.getString("date")
                            val eventTitle = eventJsonObject.getString("title")
                            val eventDescription = eventJsonObject.getString("description")
                            val eventCheck = eventJsonObject.getBoolean("check")

                            val event = Event(
                                eventDate,
                                eventTitle,
                                eventDescription,
                                eventCheck
                            )
                            eventList.add(event)
                        }

                        val calendar = Calendar(calendarName, eventList)
                        calendarList.add(calendar)
                    }
                }

                CoroutineScope(Dispatchers.Main).launch{
                    val currentDate = findViewById<TextView>(R.id.currentDate)
                    val initEvents = mutableListOf<Event>()

                    for (calendar in calendarList) {
                        if (calendar.name == title.text) {
                            val events = calendar.events.filter { it.date == currentDate.text }
                            initEvents.addAll(events)
                        }
                    }
                    val initAdapter = MainActivity.CustomAdapter(this@MainActivity, initEvents)
                    val initList = findViewById<ListView>(R.id.todolist)
                    initList.adapter = initAdapter
                }
            }
        })
    }
}