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
import android.widget.BaseAdapter
import android.widget.Button
import android.widget.ImageView
import android.widget.ListView
import android.widget.TextView
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

class StartActivity : AppCompatActivity() {

    private val calendarNameMap = mutableMapOf<String, String>()

    companion object{
        const val EXT_NAME = "calendar_name"
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

    class CustomAdapter(private val context: Context, private val items : MutableMap<String, String>) : BaseAdapter(){
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
            val view : View = inflater.inflate(R.layout.calendarlist_item,null)

            val calendarTitle = view.findViewById<TextView>(R.id.titleItem)
            val upcoming = view.findViewById<TextView>(R.id.upcomingItem)

            val entry = items.entries.elementAt(p0)
            val tempTitle = entry.key
            val tempUpcoming = entry.value

            calendarTitle.text = tempTitle
            upcoming.text = tempUpcoming

            view.setOnClickListener {
                val intent = Intent(context, MainActivity::class.java).apply {
                    putExtra(EXT_NAME, tempTitle)
                }
                context.startActivity(intent)
            }

            // edit button and delete button
            val editBtn = view.findViewById<ImageView>(R.id.fix1)
            val deleteBtn = view.findViewById<ImageView>(R.id.delete1)
            val client = OkHttpClient()

            editBtn.setOnClickListener {
                val intent = Intent(context, FixCalendar::class.java).apply {
                    putExtra(EXT_NAME,calendarTitle.text.toString() )
                }
                context.startActivity(intent)
            }

            deleteBtn.setOnClickListener {
                // Delete button 클릭 시 삭제 요청 처리
                val alertDialogBuilder = AlertDialog.Builder(context)
                alertDialogBuilder.setTitle("Delete Confirmation")
                alertDialogBuilder.setMessage("Are you sure you want to delete?")

                alertDialogBuilder.setPositiveButton("Confirm"){ dialog, which ->
                    val jsonEventData = """
                {
                    "calendar": "$tempTitle",
                    "date": "",
                    "title": "",
                    "description": "",
                    "check": false
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
                            else{
                                (context as Activity).runOnUiThread {
                                    items.remove(tempTitle)
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
        setContentView(R.layout.activity_start)

        supportActionBar?.setTitle("DayFlow")

        val client = OkHttpClient()
        val gson = Gson()
        val url = "url_link"

        val request = Request.Builder().url(url).build()

        client.newCall(request).enqueue(object : Callback{
            override fun onFailure(call: Call, e: IOException) {
                e.printStackTrace()
            }

            override fun onResponse(call: Call, response: Response) {
                if(!response.isSuccessful) throw IOException("Unexpected code $response")

                val responseBody = response.body!!.string()

                if (responseBody != null) {
                    val jsonObject = JSONObject(responseBody)
                    val calendarListJsonArray = jsonObject.getJSONArray("calendars")

                    val calendarList = mutableListOf<Calendar>()

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

                            val event = Event(eventDate, eventTitle, eventDescription, eventCheck)
                            eventList.add(event)
                        }

                        val calendar = Calendar(calendarName, eventList)
                        calendarList.add(calendar)
                    }

                    for (calendar in calendarList) {
                        val calendarName = calendar.name
                        val events = calendar.events
                        var chk = 0

                        for (event in events) {
                            val eventDate = event.date
                            val eventTitle = event.title
                            val eventDescription = event.description
                            val eventCheck = event.check

                            // 날짜 비교 및 처리
                            val currentDate = LocalDate.now()
                            val targetDate = LocalDate.parse(eventDate)
                            val comparisonResult = currentDate.compareTo(targetDate)

                            if (comparisonResult <= 0) {
                                calendarNameMap[calendarName] = "$eventDate $eventTitle : $eventDescription"
                                chk = 1
                                break
                            }
                        }
                        if(chk == 0)
                            calendarNameMap[calendarName] = "No upcoming event"
                    }
                }

                CoroutineScope(Dispatchers.Main).launch {
                    val calendarAdapter = CustomAdapter(this@StartActivity, calendarNameMap)
                    val calendarList = findViewById<ListView>(R.id.calendarList)
                    calendarList.adapter = calendarAdapter
                }
            }
        })

        // Add Calendar
        val addCalendarBnt = findViewById<ImageView>(R.id.addCalendar)

        addCalendarBnt.setOnClickListener {
            val intent = Intent(this, AddCalendar::class.java)
            startActivity(intent)
        }

    }

    override fun onResume() {
        super.onResume()
        val client = OkHttpClient()
        val gson = Gson()
        val url = "url_link"

        val request = Request.Builder().url(url).build()

        calendarNameMap.clear()

        client.newCall(request).enqueue(object : Callback{
            override fun onFailure(call: Call, e: IOException) {
                e.printStackTrace()
            }

            override fun onResponse(call: Call, response: Response) {
                if(!response.isSuccessful) throw IOException("Unexpected code $response")

                val responseBody = response.body!!.string()

                if (responseBody != null) {
                    val jsonObject = JSONObject(responseBody)
                    val calendarListJsonArray = jsonObject.getJSONArray("calendars")

                    val calendarList = mutableListOf<Calendar>()

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

                            val event = Event(eventDate, eventTitle, eventDescription, eventCheck)
                            eventList.add(event)
                        }

                        val calendar = Calendar(calendarName, eventList)
                        calendarList.add(calendar)
                    }

                    for (calendar in calendarList) {
                        val calendarName = calendar.name
                        val events = calendar.events
                        var chk = 0

                        for (event in events) {
                            val eventDate = event.date
                            val eventTitle = event.title
                            val eventDescription = event.description
                            val eventCheck = event.check

                            // 날짜 비교 및 처리
                            val currentDate = LocalDate.now()
                            val targetDate = LocalDate.parse(eventDate)
                            val comparisonResult = currentDate.compareTo(targetDate)

                            if (comparisonResult <= 0) {
                                calendarNameMap[calendarName] = "$eventDate $eventTitle : $eventDescription"
                                chk = 1
                                break
                            }
                        }
                        if(chk == 0)
                            calendarNameMap[calendarName] = "No upcoming event"
                    }
                }

                CoroutineScope(Dispatchers.Main).launch {
                    val calendarAdapter = CustomAdapter(this@StartActivity, calendarNameMap)
                    val calendarList = findViewById<ListView>(R.id.calendarList)
                    calendarList.adapter = calendarAdapter
                }
            }
        })
    }

}